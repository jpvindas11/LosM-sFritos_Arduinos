/// @copyright Los Más Fritos - 2025

#include "FileSystem.hpp"
#include <cstring>

FileSystem::FileSystem() 
: isMounted(false)
{

};

FileSystem::~FileSystem() {
    if (isMounted) {
        unmount();
    }
};

bool FileSystem::format(const std::string& path) {
    std::cout << "Formateando disco: " << path << std::endl;
    
    // Crear archivo nuevo (truncar si existe)
    std::ofstream newDisk(path, std::ios::binary | std::ios::trunc);
    
    if (!newDisk.is_open()) {
        std::cerr << "No se pudo crear el archivo del disco" << std::endl;
        return false;
    }
    
    // Escribir super bloque
    SuperBlock sb = initSuperBlock();

    char superBlockBuffer[BLOCK_SIZE];
    memset(superBlockBuffer, 0, BLOCK_SIZE);

    // Copiar superbloque al buffer
    memcpy(superBlockBuffer, &sb, sizeof(SuperBlock));
    
    // Escribir el bloque 0 completo
    newDisk.write(superBlockBuffer, BLOCK_SIZE);
    
    // Bitmap de inodos
    char zeroBitmap[BLOCK_SIZE] = {0};
    
    for (uint32_t i = 0; i < INODE_BITMAP_BLOCKS; i++) {
        newDisk.write(zeroBitmap, BLOCK_SIZE);
    }
    
    
    // Bitmap de bloques
    uint32_t systemBlocks = DATA_BLOCKS_START;
    
    // Crear bitmap de bloques
    uint8_t blockBitmapTemp[BLOCK_BITMAP_BLOCKS * BLOCK_SIZE];
    memset(blockBitmapTemp, 0, BLOCK_BITMAP_BLOCKS * BLOCK_SIZE);
    
    // Marcar bloques del sistema como ocupados
    for (uint32_t i = 0; i < systemBlocks; i++) {
        uint32_t byteIndex = i / 8;
        uint8_t bitIndex = i % 8;
        blockBitmapTemp[byteIndex] |= (1 << bitIndex);
    }

    // Escribir bitmap de bloques
    newDisk.write(reinterpret_cast<char*>(blockBitmapTemp), 
                  BLOCK_BITMAP_BLOCKS * BLOCK_SIZE);
    
    
    // Tabla de inodos
    iNode emptyInode = initInode();

    char inodeBlock[BLOCK_SIZE];
    for (uint32_t block = 0; block < INODE_TABLE_BLOCKS; block++) {
        memset(inodeBlock, 0, BLOCK_SIZE);
        // Escribir 2 inodos por bloque
        for (int i = 0; i < INODES_PER_BLOCK; i++) {
            uint32_t offset = i * (BLOCK_SIZE / INODES_PER_BLOCK);
            memcpy(inodeBlock + offset, &emptyInode, sizeof(iNode));
        }
        newDisk.write(inodeBlock, BLOCK_SIZE);
    }
    
    // Directorio
    File emptyFile;
    memset(emptyFile.fileName, 0, FILE_NAME_LENGTH);
    emptyFile.iNodePointer = INODE_FREE_SLOT;
    emptyFile.isOpen = false;
    emptyFile.type = 0;
    
    char directoryBlock[BLOCK_SIZE];
    memset(directoryBlock, 0, BLOCK_SIZE);
    int filesPerBlock = BLOCK_SIZE / sizeof(File);
    
    for (uint32_t block = 0; block < FILE_DIRECTORY_BLOCKS; block++) {
        char* ptr = directoryBlock;
        for (int i = 0; i < filesPerBlock; i++) {
            memcpy(ptr, &emptyFile, sizeof(File));
            ptr += sizeof(File);
        }
        newDisk.write(directoryBlock, BLOCK_SIZE);
    }
    
    
    // El resto del espacio se llena de 0
    char zeroBlock[BLOCK_SIZE] = {0};
    
    for (uint32_t i = 0; i < TOTAL_DATA_BLOCKS; i++) {
        newDisk.write(zeroBlock, BLOCK_SIZE);
    }

    newDisk.flush();
    newDisk.close();
    
    // Verificar tamaño final
    std::ifstream checkFile(path, std::ios::binary | std::ios::ate);
    std::streamsize fileSize = checkFile.tellg();
    checkFile.close();
    
    if (fileSize != DISK_SIZE) {
        std::cerr << "Advertencia: Tamaño del disco no coincide" << std::endl;
        std::cerr << "Esperado: " << DISK_SIZE << " bytes" << std::endl;
        std::cerr << "Obtenido: " << fileSize << " bytes" << std::endl;
    }
    
    std::cout << "Disco formateado exitosamente" << std::endl;
    std::cout << "Tamaño: " << DISK_SIZE / (1024 * 1024) << " MB" << std::endl;
    std::cout << "Bloques totales: " << TOTAL_BLOCK << std::endl;
    std::cout << "Bloques de datos: " << TOTAL_DATA_BLOCKS << std::endl;
    std::cout << "Inodos disponibles: " << TOTAL_I_NODES << std::endl;
    
    return true;
}

bool FileSystem::mount(const std::string& path) {
    if (isMounted) {
        std::cerr << "Ya hay un disco montado" << std::endl;
        return false;
    }
    this->diskPath = path;

    this->superBlock = initSuperBlock();

    // Asumimos temporalmente para manejar lecturas
    this->isMounted = true;
 
    // Abrir en modo binario, lectura y escritura
    diskFile.open(path, std::ios::in | std::ios::out | std::ios::binary);
    
    if (!diskFile.is_open()) {
        std::cerr << "Disco no existe, intentando crear uno nuevo: " << path << std::endl;

        if (!this->format(path)) {
            std::cerr << "No se pudo crear el disco: " << path << std::endl;
            this->isMounted = false;
            return false;
        }

        diskFile.clear();

        diskFile.open(path, std::ios::in | std::ios::out | std::ios::binary);
    }

    char superBlockBuffer[BLOCK_SIZE];
    if (!readBlock(0, superBlockBuffer)) {
        std::cerr << "Error al leer superbloque" << std::endl;
        diskFile.close();
        this->isMounted = false;
        return false;
    }

    // Copiar solo lo necesario
    memcpy(&superBlock, superBlockBuffer, sizeof(SuperBlock));

    // Verificar magic number
    if (superBlock.magicNumber != 0xAA55) {
        std::cerr << "Disco inválido (magic number incorrecto)" << std::endl;
        diskFile.close();
        this->isMounted = false;
        return false;
    }

    // Cargar bitmaps a memoria
    if (!loadBitmaps()) {
        std::cerr << "Error al cargar bitmaps" << std::endl;
        diskFile.close();
        this->isMounted = false;
        return false;
    }

    return true;
}

bool FileSystem::unmount() {
    if (!isMounted) {
        std::cerr << "No hay disco montado" << std::endl;
        return false;
    }
    
    // Crear un buffer de bloque completo para el superbloque
    char superBlockBuffer[BLOCK_SIZE] = {0};
    
    // Copiar el superbloque al inicio del buffer
    memcpy(superBlockBuffer, &superBlock, sizeof(SuperBlock));
    
    // Guardar el bloque completo
    if (!writeBlock(0, superBlockBuffer)) {
        std::cerr << "Error al guardar superbloque" << std::endl;
        return false;
    }
    
    // Guardar bitmaps
    if (!saveBitmaps()) {
        std::cerr << "Error al guardar bitmaps" << std::endl;
        return false;
    }
    
    diskFile.flush();
    
    diskFile.close();
    isMounted = false;
    
    std::cout << "Disco desmontado exitosamente" << std::endl;
    return true;
}

bool FileSystem::isBitSet(uint8_t* bitmap, uint32_t index) {
    uint32_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;
    // Usamos el bitIndex como el bit del número de 8 bits 
    // Presente en esa posición del array
    return (bitmap[byteIndex] & (1 << bitIndex)) != 0;
}

void FileSystem::setBit(uint8_t* bitmap, uint32_t index) {
    uint32_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;
    bitmap[byteIndex] |= (1 << bitIndex);
}

void FileSystem::clearBit(uint8_t* bitmap, uint32_t index) {
    uint32_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;
    bitmap[byteIndex] &= ~(1 << bitIndex);
}

int64_t FileSystem::findFreeBit(uint8_t* bitmap, uint32_t maxBits) {
    for (uint32_t i = 0; i < maxBits; i++) {
        if (!isBitSet(bitmap, i)) {
            return i;  // Encontrado bit libre
        }
    }
    return -1;  // No hay bits libres
}

int32_t FileSystem::allocateInode() {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return -1;
    }

    // Look for free space
    int32_t freeInode = this->findFreeBit(this->inodeBitmap, TOTAL_I_NODES);

    if (freeInode == -1) {
        std::cerr << "No hay más inodos libres" << std::endl;
        return -1;   
    }

    // Set inode
    setBit(this->inodeBitmap, freeInode);

    // Update superblock
    this->superBlock.freeInodes--;

    return freeInode;
}

void FileSystem::freeInode(uint16_t inodeNum) {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return;
    }

    if (!isBitSet(this->inodeBitmap, inodeNum)) {
        std::cerr << "El inodo ya está libre, no se puede liberar" << std::endl;
        return;
    }

    // Free bit
    clearBit(this->inodeBitmap, inodeNum);

    // Update superblock
    this->superBlock.freeInodes++;
}

int64_t FileSystem::allocateBlock() {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return -1;
    }

    int64_t freeBlock = findFreeBit(blockBitmap, TOTAL_BLOCK);
    
    if (freeBlock == -1) {
        std::cerr << "No hay más bloques libres" << std::endl;
        return -1;
    }
    
    // Asegurar no asignar bloques de los metadatos del file system
    if (freeBlock < DATA_BLOCKS_START) {
        // Buscar desde DATA_BLOCKS_START
        for (uint32_t i = DATA_BLOCKS_START; i < TOTAL_BLOCK; i++) {
            if (!isBitSet(blockBitmap, i)) {
                freeBlock = i;
                break;
            }
        }
        
        if (freeBlock < DATA_BLOCKS_START) {
            return -1;
        }
    }
    
    setBit(blockBitmap, freeBlock);
    superBlock.freeBlocks--;

    // Limpiar el bloque
    char zeroBlock[BLOCK_SIZE] = {0};
    writeBlock(freeBlock, zeroBlock);
    
    return freeBlock;
}

void FileSystem::freeBlock(uint32_t blockNum) {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return;
    }
    
    if (blockNum < DATA_BLOCKS_START) {
        std::cerr << "No se pueden liberar bloques del sistema" << std::endl;
        return;
    }
    
    if (!isBitSet(blockBitmap, blockNum)) {
        std::cerr << "El bloque ya está libre" << std::endl;
        return;
    }
    
    clearBit(blockBitmap, blockNum);
    superBlock.freeBlocks++;
}

bool FileSystem::allocateIndirectBlock(uint32_t indirectBlockNum, uint32_t index, uint32_t& allocatedBlock) {
    uint32_t pointersPerBlock = BLOCK_SIZE / sizeof(uint32_t);  // 64
    
    if (index >= pointersPerBlock) {
        std::cerr << "Índice fuera de rango" << std::endl;
        return false;
    }
    
    // Leer el bloque de punteros
    uint32_t pointerBlock[64];
    if (!readBlock(indirectBlockNum, pointerBlock)) {
        std::cerr << "Error al leer bloque indirecto" << std::endl;
        return false;
    }
    
    // Si ya hay un bloque asignado, retornarlo
    if (pointerBlock[index] != 0 && pointerBlock[index] != BLOCK_FREE_SLOT) {
        allocatedBlock = pointerBlock[index];
        return true;
    }
    
    // Asignar nuevo bloque
    int64_t newBlock = allocateBlock();
    if (newBlock == -1) {
        return false;
    }
    
    // Actualizar array y escribir de vuelta
    pointerBlock[index] = static_cast<uint32_t>(newBlock);
    allocatedBlock = static_cast<uint32_t>(newBlock);
    
    if (!writeBlock(indirectBlockNum, pointerBlock)) {
        // Rollback: liberar el bloque que acabamos de asignar en caso de error
        freeBlock(newBlock);
        return false;
    }
    
    return true;
}

bool FileSystem::freeIndirectBlocks(uint32_t indirectPointer, bool isDouble) {
    // Validar que el puntero sea válido
    if (indirectPointer == 0 || indirectPointer == BLOCK_FREE_SLOT) {
        return true;
    }
    
    if (indirectPointer < DATA_BLOCKS_START || indirectPointer >= TOTAL_BLOCK) {
        std::cerr << "Puntero indirecto inválido: " << indirectPointer << std::endl;
        return false;
    }
    
    uint32_t pointersPerBlock = BLOCK_SIZE / sizeof(uint32_t);
    
    // Leer el bloque de punteros
    uint32_t pointerBlock[64];
    if (!readBlock(indirectPointer, pointerBlock)) {
        std::cerr << "Error al leer bloque indirecto" << std::endl;
        return false;
    }
    
    // Recorrer todos los punteros en el bloque
    for (uint32_t i = 0; i < pointersPerBlock; i++) {
        uint32_t pointer = pointerBlock[i];
        
        // Si el puntero está vacío, continuar
        if (pointer == 0 || pointer == BLOCK_FREE_SLOT) {
            continue;
        }
        
        // Validar que el puntero sea válido
        if (pointer < DATA_BLOCKS_START || pointer >= TOTAL_BLOCK) {
            std::cerr << "Puntero inválido encontrado: " << pointer << std::endl;
            continue;
        }
        
        if (isDouble) {
            // Llamada recursiva para liberar el nivel indirecto simple
            freeIndirectBlocks(pointer, false);
        } else {
            // Es un indirecto simple: el puntero apunta directamente a datos
            freeBlock(pointer);
        }
    }

    freeBlock(indirectPointer);
    
    return true;
}

uint32_t FileSystem::getBlockNumber(iNode& node, uint32_t logicalBlockIndex) {
    // Bloques directos (0-11)
    if (logicalBlockIndex < MAX_DIRECT_BLOCKS) {
        return node.directBlocks[logicalBlockIndex];
    }
    
    // Indirecto simple (12-75)
    if (logicalBlockIndex < MAX_SINGLE_INDIRECT_BLOCKS) {
        if (node.singleIndirectPointer == BLOCK_FREE_SLOT || node.singleIndirectPointer == 0) {
            return BLOCK_FREE_SLOT;
        }
        
        uint32_t index = logicalBlockIndex - MAX_DIRECT_BLOCKS;
        
        // Leer bloque de punteros
        uint32_t pointerBlock[POINTERS_PER_BLOCK];
        if (!readBlock(node.singleIndirectPointer, pointerBlock)) {
            return BLOCK_FREE_SLOT;
        }
        
        return pointerBlock[index];
    }
    
    // Indirecto doble (76-4171)
    if (logicalBlockIndex < MAX_DOUBLE_INDIRECT_BLOCKS) {
        if (node.doubleIndirectPointer == BLOCK_FREE_SLOT || node.doubleIndirectPointer == 0) {
            return BLOCK_FREE_SLOT;
        }
        
        uint32_t index = logicalBlockIndex - MAX_SINGLE_INDIRECT_BLOCKS;
        uint32_t firstLevelIndex = index / POINTERS_PER_BLOCK;
        uint32_t secondLevelIndex = index % POINTERS_PER_BLOCK;
        
        // Leer primer nivel
        uint32_t firstLevelBlock[POINTERS_PER_BLOCK];
        if (!readBlock(node.doubleIndirectPointer, firstLevelBlock)) {
            return BLOCK_FREE_SLOT;
        }
        
        uint32_t secondLevelPointer = firstLevelBlock[firstLevelIndex];
        if (secondLevelPointer == BLOCK_FREE_SLOT || secondLevelPointer == 0) {
            return BLOCK_FREE_SLOT;
        }
        
        // Leer segundo nivel
        uint32_t secondLevelBlock[POINTERS_PER_BLOCK];
        if (!readBlock(secondLevelPointer, secondLevelBlock)) {
            return BLOCK_FREE_SLOT;
        }
        
        return secondLevelBlock[secondLevelIndex];
    }
    
    std::cerr << "Índice de bloque fuera de rango: " << logicalBlockIndex << std::endl;
    return BLOCK_FREE_SLOT;
}

bool FileSystem::setBlockNumber(iNode& node, uint32_t logicalBlockIndex, uint32_t physicalBlock) {
    // Bloques directos (0-11)
    if (logicalBlockIndex < MAX_DIRECT_BLOCKS) {
        node.directBlocks[logicalBlockIndex] = physicalBlock;
        return true;
    }
    
    // Indirecto simple (12-75)
    if (logicalBlockIndex < MAX_SINGLE_INDIRECT_BLOCKS) {
        // Asignar bloque de punteros si no existe
        if (node.singleIndirectPointer == BLOCK_FREE_SLOT || node.singleIndirectPointer == 0) {
            int64_t newBlock = allocateBlock();
            if (newBlock == -1) {
                std::cerr << "No se pudo asignar bloque para puntero indirecto simple" << std::endl;
                return false;
            }
            node.singleIndirectPointer = static_cast<uint32_t>(newBlock);
            
            // Inicializar bloque de punteros
            uint32_t emptyPointers[POINTERS_PER_BLOCK];
            for (size_t i = 0; i < POINTERS_PER_BLOCK; i++) {
                emptyPointers[i] = BLOCK_FREE_SLOT;
            }
            writeBlock(node.singleIndirectPointer, emptyPointers);
        }
        
        uint32_t index = logicalBlockIndex - MAX_DIRECT_BLOCKS;
        
        // Leer, modificar y escribir bloque de punteros
        uint32_t pointerBlock[POINTERS_PER_BLOCK];
        if (!readBlock(node.singleIndirectPointer, pointerBlock)) {
            return false;
        }
        
        pointerBlock[index] = physicalBlock;
        
        if (!writeBlock(node.singleIndirectPointer, pointerBlock)) {
            return false;
        }
        
        return true;
    }
    
    // Indirecto doble (76-4171)
    if (logicalBlockIndex < MAX_DOUBLE_INDIRECT_BLOCKS) {
        // Asignar bloque de primer nivel si no existe
        if (node.doubleIndirectPointer == BLOCK_FREE_SLOT || node.doubleIndirectPointer == 0) {
            int64_t newBlock = allocateBlock();
            if (newBlock == -1) {
                std::cerr << "No se pudo asignar bloque para puntero indirecto doble" << std::endl;
                return false;
            }
            node.doubleIndirectPointer = static_cast<uint32_t>(newBlock);
            
            // Inicializar primer nivel
            uint32_t emptyPointers[POINTERS_PER_BLOCK];
            for (size_t i = 0; i < POINTERS_PER_BLOCK; i++) {
                emptyPointers[i] = BLOCK_FREE_SLOT;
            }
            writeBlock(node.doubleIndirectPointer, emptyPointers);
        }
        
        uint32_t index = logicalBlockIndex - MAX_SINGLE_INDIRECT_BLOCKS;
        uint32_t firstLevelIndex = index / POINTERS_PER_BLOCK;
        uint32_t secondLevelIndex = index % POINTERS_PER_BLOCK;
        
        // Leer primer nivel
        uint32_t firstLevelBlock[POINTERS_PER_BLOCK];
        if (!readBlock(node.doubleIndirectPointer, firstLevelBlock)) {
            return false;
        }
        
        // Asignar bloque de segundo nivel si no existe
        if (firstLevelBlock[firstLevelIndex] == BLOCK_FREE_SLOT || firstLevelBlock[firstLevelIndex] == 0) {
            int64_t newBlock = allocateBlock();
            if (newBlock == -1) {
                std::cerr << "No se pudo asignar bloque de segundo nivel" << std::endl;
                return false;
            }
            firstLevelBlock[firstLevelIndex] = static_cast<uint32_t>(newBlock);
            
            // Guardar primer nivel actualizado
            if (!writeBlock(node.doubleIndirectPointer, firstLevelBlock)) {
                freeBlock(newBlock);
                return false;
            }
            
            // Inicializar segundo nivel
            uint32_t emptyPointers[POINTERS_PER_BLOCK];
            for (size_t i = 0; i < POINTERS_PER_BLOCK; i++) {
                emptyPointers[i] = BLOCK_FREE_SLOT;
            }
            writeBlock(newBlock, emptyPointers);
        }
        
        // Leer, modificar y escribir segundo nivel
        uint32_t secondLevelBlock[POINTERS_PER_BLOCK];
        if (!readBlock(firstLevelBlock[firstLevelIndex], secondLevelBlock)) {
            return false;
        }
        
        secondLevelBlock[secondLevelIndex] = physicalBlock;
        
        if (!writeBlock(firstLevelBlock[firstLevelIndex], secondLevelBlock)) {
            return false;
        }
        
        return true;
    }
    
    std::cerr << "Índice de bloque fuera de rango: " << logicalBlockIndex << std::endl;
    return false;
}

FileLocation FileSystem::findFileInDirectory(const std::string& fileName) {
    FileLocation result;
    result.found = false;

    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return result;
    }
    
    if (fileName.empty() || fileName.length() >= FILE_NAME_LENGTH) {
        std::cerr << "Nombre de archivo inválido" << std::endl;
        return result;
    }
    
    uint32_t filesPerBlock = BLOCK_SIZE / sizeof(File);
    
    for (uint32_t block = 0; block < FILE_DIRECTORY_BLOCKS; block++) {
        char buffer[BLOCK_SIZE];
        if (!readBlock(FILE_DIRECTORY_START + block, buffer)) {
            return result;
        }
        
        for (uint32_t i = 0; i < filesPerBlock; i++) {
            File* entry = reinterpret_cast<File*>(buffer + (i * sizeof(File)));
            
            if (entry->iNodePointer != INODE_FREE_SLOT &&
                strncmp(entry->fileName, fileName.c_str(), FILE_NAME_LENGTH - 1) == 0) {
                
                result.file = *entry;
                result.blockNum = FILE_DIRECTORY_START + block;
                result.offset = i * sizeof(File);
                result.found = true;
                return result;
            }
        }
    }
    return result;
}

bool FileSystem::addFileToDirectory(const std::string& fileName, uint16_t inodeNum) {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return false;
    }
    
    // Validar nombre
    if (fileName.empty() || fileName.length() >= FILE_NAME_LENGTH) {
        std::cerr << "Nombre de archivo inválido (máximo " << FILE_NAME_LENGTH - 1 << " caracteres)" << std::endl;
        return false;
    }
    
    // Verificar si el archivo ya existe
    if (findFileInDirectory(fileName).found) {
        std::cerr << "El archivo ya existe: " << fileName << std::endl;
        return false;
    }
    
    // Buscar una entrada libre en el directorio
    uint32_t filesPerBlock = BLOCK_SIZE / sizeof(File);
    
    for (uint32_t block = 0; block < FILE_DIRECTORY_BLOCKS; block++) {
        char buffer[BLOCK_SIZE];
        if (!readBlock(FILE_DIRECTORY_START + block, buffer)) {
            return false;
        }
        
        for (uint32_t i = 0; i < filesPerBlock; i++) {
            File* entry = reinterpret_cast<File*>(buffer + (i * sizeof(File)));
            
            if (entry->iNodePointer == INODE_FREE_SLOT) {
                // Espacio libre encontrado
                memset(entry->fileName, 0, FILE_NAME_LENGTH);
                strncpy(entry->fileName, fileName.c_str(), FILE_NAME_LENGTH - 1);
                entry->iNodePointer = inodeNum;
                entry->isOpen = false;
                entry->type = FT_FILE;
                
                return writeBlock(FILE_DIRECTORY_START + block, buffer);
            }
        }
    }
    
    std::cerr << "Directorio lleno" << std::endl;
    return false;
}

bool FileSystem::removeFileFromDirectory(const std::string& fileName) {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return false;
    }
    
    FileLocation location = findFileInDirectory(fileName);
    if (!location.found) {
        std::cerr << "El archivo no existe: " << fileName << std::endl;
        return false;
    }
    
    // Leer el bloque
    char buffer[BLOCK_SIZE];
    if (!readBlock(location.blockNum, buffer)) {
        return false;
    }
    
    // Acceder a la entrada y limpiarla
    File* entry = reinterpret_cast<File*>(buffer + location.offset);
    memset(entry->fileName, 0, FILE_NAME_LENGTH);
    entry->iNodePointer = INODE_FREE_SLOT;
    entry->isOpen = false;
    entry->type = 0;
    
    return writeBlock(location.blockNum, buffer);
} 

bool FileSystem::readBlock(uint32_t blockNum, void* buffer) {
    if (!isMounted && blockNum != 0) {
        std::cerr << "    ERROR: Disco no montado" << std::endl;
        return false;
    }
    
    if (blockNum >= TOTAL_BLOCK) {
        std::cerr << "    ERROR: Número de bloque inválido: " << blockNum << std::endl;
        return false;
    }
    
    if (!diskFile.is_open()) {
        std::cerr << "    ERROR: diskFile NO está abierto!" << std::endl;
        return false;
    }
    
    // Calcular posición en el archivo
    std::streampos position = static_cast<std::streampos>(blockNum) * BLOCK_SIZE;
    // Mover puntero de lectura
    diskFile.seekg(position);
    
    if (diskFile.fail()) {
        std::cerr << "    ERROR: Error al posicionar puntero de lectura" << std::endl;
        diskFile.clear(); // Limpiar flags de error
        return false;
    }
    
    // Obtener posición actual para verificar
    std::streampos actualPos = diskFile.tellg();
    
    // Leer bloque completo
    diskFile.read(static_cast<char*>(buffer), BLOCK_SIZE);

    if (diskFile.fail()) {
        std::cerr << "    ERROR: Error al leer bloque " << blockNum << std::endl;
        diskFile.clear(); // Limpiar flags de error
        return false;
    }

    return true;
}


bool FileSystem::writeBlock(uint32_t blockNum, const void* buffer) {
    if (!isMounted && blockNum != 0) {
        std::cerr << "Disco no montado" << std::endl;
        return false;
    }
    
    if (blockNum >= TOTAL_BLOCK) {
        std::cerr << "Número de bloque inválido: " << blockNum << std::endl;
        return false;
    }

    // Calcular posición en el archivo
    std::streampos position = static_cast<std::streampos>(blockNum) * BLOCK_SIZE;

    // Mover puntero de escritura
    diskFile.seekp(position);
    
    if (diskFile.fail()) {
        std::cerr << "Error al posicionar puntero de escritura" << std::endl;
        return false;
    }
    
    // Escribir bloque completo
    diskFile.write(static_cast<const char*>(buffer), BLOCK_SIZE);
    
    if (diskFile.fail()) {
        std::cerr << "Error al escribir bloque " << blockNum << std::endl;
        return false;
    }
    
    // Flush para asegurar escritura (opcional, pero recomendado)
    diskFile.flush();
    
    return true;
}

bool FileSystem::loadBitmaps() {
    char* inodeBitmapPtr = reinterpret_cast<char*>(inodeBitmap);
    for (uint32_t i = 0; i < INODE_BITMAP_BLOCKS; i++) {
        
        if (!readBlock(INODE_BITMAP_START + i, inodeBitmapPtr + (i * BLOCK_SIZE))) {
            std::cerr << "ERROR: False en inodos, bloque: " << (INODE_BITMAP_START + i) << std::endl;
            return false;
        }
    }

    char* blockBitmapPtr = reinterpret_cast<char*>(blockBitmap);
    for (uint32_t i = 0; i < BLOCK_BITMAP_BLOCKS; i++) {
        if (!readBlock(BLOCK_BITMAP_START + i, blockBitmapPtr + (i * BLOCK_SIZE))) {
            std::cerr << "ERROR: False en bloques, bloque: " << (BLOCK_BITMAP_START + i) << std::endl;
            return false;
        }
    }

    return true;
}

bool FileSystem::saveBitmaps() {
    // Guardar bitmap de inodos
    char* inodeBitmapPtr = reinterpret_cast<char*>(inodeBitmap);
    for (uint32_t i = 0; i < INODE_BITMAP_BLOCKS; i++) {
        if (!writeBlock(INODE_BITMAP_START + i, inodeBitmapPtr + (i * BLOCK_SIZE))) {
            return false;
        }
    }
    // Guardar bitmap de bloques
    char* blockBitmapPtr = reinterpret_cast<char*>(blockBitmap);
    for (uint32_t i = 0; i < BLOCK_BITMAP_BLOCKS; i++) {
        if (!writeBlock(BLOCK_BITMAP_START + i, blockBitmapPtr + (i * BLOCK_SIZE))) {
            return false;
        }
    }
    
    return true;
}

bool FileSystem::readInode(uint16_t inodeNum, iNode* inode) {
    
    if (inodeNum >= TOTAL_I_NODES || !inode) {
        std::cout << "Inodo fuera de los limites de memoria" << std::endl;
        return false;
    }
    
    // Calcular bloque y posición dentro del bloque (2 inodos por bloque)
    uint32_t inodeBlockIndex = inodeNum / INODES_PER_BLOCK;
    uint32_t inodeSlot = inodeNum % INODES_PER_BLOCK;
    uint32_t blockNum = INODE_TABLE_START + inodeBlockIndex;
    uint32_t offset = inodeSlot * (BLOCK_SIZE / INODES_PER_BLOCK);
    
    char buffer[BLOCK_SIZE];
    if (!readBlock(blockNum, buffer)) return false;
    
    memcpy(inode, buffer + offset, sizeof(iNode));
    
    return true;
}

bool FileSystem::writeInode(uint16_t inodeNum, const iNode* inode) {
    
    if (inodeNum >= TOTAL_I_NODES || !inode) {
        return false;
    }
    
    // Calcular bloque y posición dentro del bloque (2 inodos por bloque)
    uint32_t inodeBlockIndex = inodeNum / INODES_PER_BLOCK;
    uint32_t inodeSlot = inodeNum % INODES_PER_BLOCK;
    uint32_t blockNum = INODE_TABLE_START + inodeBlockIndex;
    uint32_t offset = inodeSlot * (BLOCK_SIZE / INODES_PER_BLOCK);

    char buffer[BLOCK_SIZE];

    if (!readBlock(blockNum, buffer)) {
        memset(buffer, 0, BLOCK_SIZE);
    }

    memcpy(buffer + offset, inode, sizeof(iNode));
    
    bool result = writeBlock(blockNum, buffer);

    return result;
}

bool FileSystem::createFile(const std::string& fileName, uint16_t permissions) {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return false;
    }

    // Validar nombre
    if (fileName.empty() || fileName.length() >= FILE_NAME_LENGTH) {
        std::cerr << "Nombre de archivo inválido (máximo " << FILE_NAME_LENGTH - 1 << " caracteres)" << std::endl;
        return false;
    }

    // Verificar que el archivo no exista
    if (findFileInDirectory(fileName).found) {
        std::cerr << "El archivo ya existe: " << fileName << std::endl;
        return false;
    }
    
    // Asignar un inodo
    int32_t inodeNum = allocateInode();
    if (inodeNum == -1) {
        std::cerr << "No se pudo asignar inodo" << std::endl;
        return false;
    }

    // Inicializar el inodo
    iNode node = initInode();
    node.size = 0;
    node.permissions = permissions;
    node.userId = 0;
    node.groupId = 0;
    node.creationTime = time(nullptr);
    node.lastModifiedTime = node.creationTime;
    node.lastAccessTime = node.creationTime;
    
    // Marcar punteros como libres
    for (int i = 0; i < TOTAL_DIRECT_POINTERS; i++) {
        node.directBlocks[i] = BLOCK_FREE_SLOT;
    }
    node.singleIndirectPointer = BLOCK_FREE_SLOT;
    node.doubleIndirectPointer = BLOCK_FREE_SLOT;
    
    int64_t firstBlock = allocateBlock();
    if (firstBlock != -1) {
        node.directBlocks[0] = static_cast<uint32_t>(firstBlock);

        char emptyBlock[BLOCK_SIZE] = {0};
        if (!writeBlock(firstBlock, emptyBlock)) {
            std::cerr << "Error al inicializar bloque" << std::endl;
            freeBlock(firstBlock);
            freeInode(inodeNum);
            return false;
        }
    }
    
    // Escribir inodo
    if (!writeInode(inodeNum, &node)) {
        std::cerr << "Error al escribir inodo" << std::endl;
        if (firstBlock != -1) {
            freeBlock(firstBlock);
        }
        freeInode(inodeNum);
        return false;
    }
    
    // Agregar al directorio
    if (!addFileToDirectory(fileName, inodeNum)) {
        std::cerr << "Error al agregar archivo al directorio" << std::endl;
        freeInode(inodeNum);
        return false;
    }

    // actualizar bitmaps
    if (!saveBitmaps()) {
        std::cerr << "Error al guardar bitmaps" << std::endl;
        removeFileFromDirectory(fileName);
        freeInode(inodeNum);
        return false;
    }
    
    // super bloque
    char superBlockBuffer[BLOCK_SIZE] = {0};
    memcpy(superBlockBuffer, &superBlock, sizeof(SuperBlock));
    if (!writeBlock(0, superBlockBuffer)) {
        std::cerr << "Error al guardar superbloque" << std::endl;
        return false;
    }
    
    std::cout << "Archivo creado: " << fileName << " (inodo " << inodeNum << ")" << std::endl;
    return true;
}

bool FileSystem::writeFile(const std::string& fileName, const char* data, uint32_t size) {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return false;
    }
    
    if (!data || size == 0) {
        std::cerr << "Datos inválidos" << std::endl;
        return false;
    }
    
    FileLocation fileIndex = findFileInDirectory(fileName);
    if (!fileIndex.found) {
        std::cerr << "Archivo no encontrado: " << fileName << std::endl;
        return false;
    }

    uint16_t inodeNum = fileIndex.file.iNodePointer;
    
    iNode node;
    if (!readInode(inodeNum, &node)) {
        std::cerr << "Error al leer inodo" << std::endl;
        return false;
    }

    uint32_t blocksNeeded = BLOCKS_NEEDED(size);
    
    // Verificar límite máximo
    if (blocksNeeded > MAX_DOUBLE_INDIRECT_BLOCKS) {
        std::cerr << "Archivo demasiado grande. Máximo: " 
                  << (MAX_DOUBLE_INDIRECT_BLOCKS * BLOCK_SIZE) << " bytes" << std::endl;
        return false;
    }

    // Liberar bloques antiguos que ya no se necesitan
    uint32_t oldBlocksNeeded = BLOCKS_NEEDED(node.size);
    for (uint32_t i = blocksNeeded; i < oldBlocksNeeded; i++) {
        uint32_t blockNum = getBlockNumber(node, i);
        if (blockNum != BLOCK_FREE_SLOT && blockNum >= DATA_BLOCKS_START) {
            freeBlock(blockNum);
        }
    }
    
    // Si el archivo se vuelve más pequeño, liberar estructuras de indirección innecesarias
    if (blocksNeeded <= MAX_DIRECT_BLOCKS && node.singleIndirectPointer != BLOCK_FREE_SLOT) {
        freeIndirectBlocks(node.singleIndirectPointer, false);
        node.singleIndirectPointer = BLOCK_FREE_SLOT;
    }
    if (blocksNeeded <= MAX_SINGLE_INDIRECT_BLOCKS && node.doubleIndirectPointer != BLOCK_FREE_SLOT) {
        freeIndirectBlocks(node.doubleIndirectPointer, true);
        node.doubleIndirectPointer = BLOCK_FREE_SLOT;
    }
    
    // Asignar bloques necesarios
    for (uint32_t i = 0; i < blocksNeeded; i++) {
        uint32_t currentBlock = getBlockNumber(node, i);
        
        if (currentBlock == BLOCK_FREE_SLOT || currentBlock == 0) {
            int64_t newBlock = allocateBlock();
            if (newBlock == -1) {
                std::cerr << "No hay bloques disponibles" << std::endl;
                return false;
            }
            
            if (!setBlockNumber(node, i, static_cast<uint32_t>(newBlock))) {
                freeBlock(newBlock);
                return false;
            }
        }
    }
    
    // Actualizar metadata
    node.size = size;
    node.lastModifiedTime = time(nullptr);
    
    if (!writeInode(inodeNum, &node)) {
        std::cerr << "Error al actualizar inodo" << std::endl;
        return false;
    }
    
    // Escribir datos
    uint32_t bytesWritten = 0;
    uint32_t currentBlockIndex = 0;
    
    while (bytesWritten < size && currentBlockIndex < blocksNeeded) {
        uint32_t blockNum = getBlockNumber(node, currentBlockIndex);
        
        if (blockNum < DATA_BLOCKS_START || blockNum >= TOTAL_BLOCK) {
            std::cerr << "Error: Bloque inválido: " << blockNum << std::endl;
            return false;
        }
        
        uint32_t bytesToWrite = std::min<uint32_t>(BLOCK_SIZE, size - bytesWritten);
        
        char blockBuffer[BLOCK_SIZE] = {0};
        memcpy(blockBuffer, data + bytesWritten, bytesToWrite);
        
        if (!writeBlock(blockNum, blockBuffer)) {
            std::cerr << "Error al escribir bloque" << std::endl;
            return false;
        }
        
        bytesWritten += bytesToWrite;
        currentBlockIndex++;
    }
    
    saveBitmaps();
    
    char superBlockBuffer[BLOCK_SIZE] = {0};
    memcpy(superBlockBuffer, &superBlock, sizeof(SuperBlock));
    writeBlock(0, superBlockBuffer);
    
    std::cout << "Escritos " << bytesWritten << " bytes en " << fileName << std::endl;
    return true;
}

bool FileSystem::appendFile(const std::string& fileName, const char* data, uint32_t size) {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return false;
    }
    
    if (!data || size == 0) {
        std::cerr << "Datos inválidos" << std::endl;
        return false;
    }
    
    FileLocation fileIndex = findFileInDirectory(fileName);
    if (!fileIndex.found) {
        std::cerr << "Archivo no encontrado: " << fileName << std::endl;
        return false;
    }

    uint16_t inodeNum = fileIndex.file.iNodePointer;
    
    iNode node;
    if (!readInode(inodeNum, &node)) {
        std::cerr << "Error al leer inodo" << std::endl;
        return false;
    }

    uint32_t oldSize = node.size;
    uint32_t newSize = oldSize + size;
    
    // Verificar límite máximo
    uint32_t newBlocksNeeded = BLOCKS_NEEDED(newSize);
    if (newBlocksNeeded > MAX_DOUBLE_INDIRECT_BLOCKS) {
        std::cerr << "Archivo resultante demasiado grande. Máximo: " 
                  << (MAX_DOUBLE_INDIRECT_BLOCKS * BLOCK_SIZE) << " bytes" << std::endl;
        return false;
    }
    
    uint32_t oldBlocksNeeded = BLOCKS_NEEDED(oldSize);
    
    // Asignar solo los bloques NUEVOS que falten
    for (uint32_t i = oldBlocksNeeded; i < newBlocksNeeded; i++) {
        int64_t newBlock = allocateBlock();
        if (newBlock == -1) {
            std::cerr << "No hay bloques disponibles" << std::endl;
            return false;
        }
        
        if (!setBlockNumber(node, i, static_cast<uint32_t>(newBlock))) {
            freeBlock(newBlock);
            return false;
        }
    }
    
    // Calcular posición inicial de escritura
    uint32_t offsetInLastBlock = (oldSize > 0) ? (oldSize % BLOCK_SIZE) : 0;
    uint32_t lastBlockIndex = (oldSize > 0) ? ((oldSize - 1) / BLOCK_SIZE) : 0;
    
    uint32_t bytesWritten = 0;
    
    // Si hay espacio en el último bloque, llenar primero ahí
    if (offsetInLastBlock > 0 && oldSize > 0) {
        uint32_t spaceInLastBlock = BLOCK_SIZE - offsetInLastBlock;
        uint32_t bytesToWriteInLastBlock = std::min(spaceInLastBlock, size);
        
        uint32_t lastBlockNum = getBlockNumber(node, lastBlockIndex);
        
        if (lastBlockNum == BLOCK_FREE_SLOT || lastBlockNum < DATA_BLOCKS_START) {
            std::cerr << "Error: Último bloque inválido" << std::endl;
            return false;
        }
        
        // Leer el bloque existente
        char blockBuffer[BLOCK_SIZE];
        if (!readBlock(lastBlockNum, blockBuffer)) {
            std::cerr << "Error al leer último bloque" << std::endl;
            return false;
        }
        
        // Copiar los nuevos datos después de los existentes
        memcpy(blockBuffer + offsetInLastBlock, data, bytesToWriteInLastBlock);
        
        // Escribir el bloque de vuelta
        if (!writeBlock(lastBlockNum, blockBuffer)) {
            std::cerr << "Error al escribir en último bloque" << std::endl;
            return false;
        }
        
        bytesWritten += bytesToWriteInLastBlock;
    }
    
    // Escribir los datos restantes en bloques nuevos/siguientes
    uint32_t currentBlockIndex = (oldSize > 0) ? (lastBlockIndex + 1) : 0;
    
    while (bytesWritten < size && currentBlockIndex < newBlocksNeeded) {
        uint32_t blockNum = getBlockNumber(node, currentBlockIndex);
        
        if (blockNum < DATA_BLOCKS_START || blockNum >= TOTAL_BLOCK) {
            std::cerr << "Error: Bloque inválido: " << blockNum << std::endl;
            return false;
        }
        
        uint32_t bytesToWrite = std::min<uint32_t>(BLOCK_SIZE, size - bytesWritten);
        
        char blockBuffer[BLOCK_SIZE] = {0};
        memcpy(blockBuffer, data + bytesWritten, bytesToWrite);
        
        if (!writeBlock(blockNum, blockBuffer)) {
            std::cerr << "Error al escribir bloque" << std::endl;
            return false;
        }
        
        bytesWritten += bytesToWrite;
        currentBlockIndex++;
    }
    
    // Actualizar metadata del inodo
    node.size = newSize;
    node.lastModifiedTime = time(nullptr);
    
    if (!writeInode(inodeNum, &node)) {
        std::cerr << "Error al actualizar inodo" << std::endl;
        return false;
    }
    
    saveBitmaps();
    
    char superBlockBuffer[BLOCK_SIZE] = {0};
    memcpy(superBlockBuffer, &superBlock, sizeof(SuperBlock));
    writeBlock(0, superBlockBuffer);
    
    std::cout << "Agregados " << bytesWritten << " bytes a " << fileName 
              << " (tamaño anterior: " << oldSize << ", nuevo: " << newSize << ")" << std::endl;
    
    return true;
}

bool FileSystem::fileExists(const std::string& fileName) {
    return findFileInDirectory(fileName).found;
}

uint32_t FileSystem::getFileSize(const std::string& fileName) {
    FileLocation loc = findFileInDirectory(fileName);
    if (!loc.found) return 0;

    iNode node;
    if (!readInode(loc.file.iNodePointer, &node)) {
        return 0;
    }
    return node.size;
}

bool FileSystem::readFile(const std::string& fileName, char* buffer, uint32_t& size) {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return false;
    }
    
    if (!buffer) {
        std::cerr << "Buffer inválido" << std::endl;
        return false;
    }
    
    FileLocation fileIndex = findFileInDirectory(fileName);
    if (!fileIndex.found) {
        std::cerr << "Archivo no encontrado: " << fileName << std::endl;
        return false;
    }

    uint16_t inodeNum = fileIndex.file.iNodePointer;
    
    iNode node;
    if (!readInode(inodeNum, &node)) {
        std::cerr << "Error al leer inodo" << std::endl;
        return false;
    }
    
    node.lastAccessTime = time(nullptr);
    writeInode(inodeNum, &node);
    
    uint32_t bytesToRead = std::min(size, node.size);
    uint32_t blocksNeeded = BLOCKS_NEEDED(bytesToRead);
    
    uint32_t bytesRead = 0;
    uint32_t currentBlockIndex = 0;
    
    while (bytesRead < bytesToRead && currentBlockIndex < blocksNeeded) {
        uint32_t blockNum = getBlockNumber(node, currentBlockIndex);
        
        if (blockNum == BLOCK_FREE_SLOT || blockNum < DATA_BLOCKS_START || blockNum >= TOTAL_BLOCK) {
            std::cerr << "Error: Bloque inválido: " << blockNum << std::endl;
            break;
        }
        
        char blockBuffer[BLOCK_SIZE];
        if (!readBlock(blockNum, blockBuffer)) {
            std::cerr << "Error al leer bloque " << blockNum << std::endl;
            return false;
        }
        
        uint32_t bytesFromBlock = std::min<uint32_t>(BLOCK_SIZE, bytesToRead - bytesRead);
        memcpy(buffer + bytesRead, blockBuffer, bytesFromBlock);
        
        bytesRead += bytesFromBlock;
        currentBlockIndex++;
    }
    
    size = bytesRead;
    
    if (bytesRead < bytesToRead) {
        std::cerr << "Advertencia: Solo se pudieron leer " << bytesRead 
                  << " de " << bytesToRead << " bytes" << std::endl;
    }
    
    return bytesRead > 0;
}

bool FileSystem::readFile(const std::string& fileName, uint32_t cursor, char* buffer, uint32_t& size) {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return false;
    }
    if (!buffer) {
        std::cerr << "Buffer inválido" << std::endl;
        return false;
    }

    FileLocation fileIndex = findFileInDirectory(fileName);
    if (!fileIndex.found) {
        std::cerr << "Archivo no encontrado: " << fileName << std::endl;
        size = 0;
        return false;
    }

    uint16_t inodeNum = fileIndex.file.iNodePointer;
    iNode node;
    if (!readInode(inodeNum, &node)) {
        std::cerr << "Error al leer inodo" << std::endl;
        size = 0;
        return false;
    }

    // Actualizar ultimo acceso
    node.lastAccessTime = time(nullptr);
    writeInode(inodeNum, &node);

    if (cursor >= node.size) {
        size = 0;
        return false;
    }

    uint32_t maxToRead = std::min(size, node.size - cursor);
    uint32_t bytesRemaining = maxToRead;
    uint32_t bytesRead = 0;

    uint32_t currentBlockIndex = cursor / BLOCK_SIZE;
    uint32_t offsetInBlock = cursor % BLOCK_SIZE;

    while (bytesRemaining > 0) {
        uint32_t physBlock = getBlockNumber(node, currentBlockIndex);
        if (physBlock == BLOCK_FREE_SLOT || physBlock < DATA_BLOCKS_START || physBlock >= TOTAL_BLOCK) {
            // Bloque inválido / no asignado
            break;
        }

        char blockBuffer[BLOCK_SIZE];
        if (!readBlock(physBlock, blockBuffer)) {
            std::cerr << "Error al leer bloque físico: " << physBlock << std::endl;
            break;
        }

        uint32_t startInBlock = (currentBlockIndex == (cursor / BLOCK_SIZE)) ? offsetInBlock : 0;
        uint32_t canTake = std::min<uint32_t>(BLOCK_SIZE - startInBlock, bytesRemaining);

        memcpy(buffer + bytesRead, blockBuffer + startInBlock, canTake);

        bytesRead += canTake;
        bytesRemaining -= canTake;
        currentBlockIndex++;
        offsetInBlock = 0;
    }

    size = bytesRead;
    return bytesRead > 0;
}

bool FileSystem::deleteFile(const std::string& fileName) {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return false;
    }
    
    FileLocation fileIndex = findFileInDirectory(fileName);
    if (!fileIndex.found) {
        std::cerr << "Archivo no encontrado: " << fileName << std::endl;
        return false;
    }

    uint16_t inodeNum = fileIndex.file.iNodePointer;
    
    iNode node;
    if (!readInode(inodeNum, &node)) {
        std::cerr << "Error al leer inodo" << std::endl;
        return false;
    }
    
    // Liberar TODOS los bloques del archivo
    uint32_t blocksUsed = BLOCKS_NEEDED(node.size);
    
    for (uint32_t i = 0; i < std::min(blocksUsed, (uint32_t)TOTAL_DIRECT_POINTERS); i++) {
        uint32_t blockNum = node.directBlocks[i];
        if (blockNum != BLOCK_FREE_SLOT && blockNum >= DATA_BLOCKS_START) {
            freeBlock(blockNum);
        }
    }
    
    // Liberar estructuras de indirección
    if (node.singleIndirectPointer != BLOCK_FREE_SLOT) {
        freeIndirectBlocks(node.singleIndirectPointer, false);
    }
    
    if (node.doubleIndirectPointer != BLOCK_FREE_SLOT) {
        freeIndirectBlocks(node.doubleIndirectPointer, true);
    }
    
    // Liberar inodo
    freeInode(inodeNum);
    
    // Remover del directorio
    if (!removeFileFromDirectory(fileName)) {
        return false;
    }
    
    // Persistir cambios
    saveBitmaps();
    
    char superBlockBuffer[BLOCK_SIZE] = {0};
    memcpy(superBlockBuffer, &superBlock, sizeof(SuperBlock));
    writeBlock(0, superBlockBuffer);
    
    std::cout << "Archivo eliminado: " << fileName << std::endl;
    return true;
}

bool FileSystem::getFileInfo(const std::string& fileName, iNode* info) {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return false;
    }
    
    FileLocation fileIndex = findFileInDirectory(fileName);
    if (!fileIndex.found) {
        std::cerr << "Archivo no encontrado: " << fileName << std::endl;
        return false;
    }

    uint16_t inodeNum = fileIndex.file.iNodePointer;
    
    iNode node;
    if (!readInode(inodeNum, &node)) {
        std::cerr << "Error al leer inodo" << std::endl;
        return false;
    }

    // Asignar informacion
    *info = node;
    return true;
}

std::vector<std::string> FileSystem::listFiles() {
    std::vector<std::string> list;
    
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return list;
    }
    
    uint32_t filesPerBlock = BLOCK_SIZE / sizeof(File);
    
    // Recorrer todos los bloques del directorio
    for (uint32_t block = 0; block < FILE_DIRECTORY_BLOCKS; block++) {
        char buffer[BLOCK_SIZE];
        
        if (!readBlock(FILE_DIRECTORY_START + block, buffer)) {
            std::cerr << "Error al leer bloque del directorio: " << block << std::endl;
            continue;  // Continuar con el siguiente bloque
        }
        
        // Revisar cada archivo en este bloque
        for (uint32_t i = 0; i < filesPerBlock; i++) {
            File* entry = reinterpret_cast<File*>(buffer + (i * sizeof(File)));
            
            // Si la entrada está ocupada, agregar a la lista
            if (entry->iNodePointer != INODE_FREE_SLOT) {
                list.push_back(entry->fileName);
            }
        }
    }
    
    return list;
}

bool FileSystem::renameFile(const std::string& oldName, const std::string& newName) {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return false;
    }

    // Validar nombre
    if (newName.empty() || newName.length() >= FILE_NAME_LENGTH) {
        std::cerr << "Nombre de archivo inválido (máximo " << FILE_NAME_LENGTH - 1 << " caracteres)" << std::endl;
        return false;
    }

    // New name ya existe
    if (findFileInDirectory(newName).found) {
        std::cerr << "Cambio de nombre de archivo a uno que ya existe" << std::endl;
        return false;
    }

    
    FileLocation fileIndex = findFileInDirectory(oldName);
    if (!fileIndex.found) {
        std::cerr << "Archivo no encontrado: " << oldName << std::endl;
        return false;
    }

    uint16_t inodeNum = fileIndex.file.iNodePointer;
    
    iNode node;
    if (!readInode(inodeNum, &node)) {
        std::cerr << "Error al leer inodo" << std::endl;
        return false;
    }

    // Colocar nombre
    memset(fileIndex.file.fileName, 0, FILE_NAME_LENGTH);
    strncpy(fileIndex.file.fileName, newName.c_str(), FILE_NAME_LENGTH - 1);
    fileIndex.file.fileName[FILE_NAME_LENGTH - 1] = '\0';

    // Actualizar tiempo de modificacion y acceso
    node.lastAccessTime = time(nullptr);
    node.lastModifiedTime = node.lastAccessTime;

    if (!writeInode(inodeNum, &node)) {
        std::cerr << "Error al actualizar inodo" << std::endl;
        return false;
    }

    return true;
}

bool FileSystem::changePermissions(const std::string& fileName, uint16_t permissions) {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return false;
    }
    
    FileLocation fileIndex = findFileInDirectory(fileName);
    if (!fileIndex.found) {
        std::cerr << "Archivo no encontrado: " << fileName << std::endl;
        return false;
    }

    uint16_t inodeNum = fileIndex.file.iNodePointer;
    
    iNode node;
    if (!readInode(inodeNum, &node)) {
        std::cerr << "Error al leer inodo" << std::endl;
        return false;
    }

    // Cambiar los permisos
    node.permissions = permissions;

    // Actualizar tiempo de modificacion y acceso
    node.lastAccessTime = time(nullptr);
    node.lastModifiedTime = node.lastAccessTime;

    if (!writeInode(inodeNum, &node)) {
        std::cerr << "Error al actualizar inodo" << std::endl;
        return false;
    }

    return true;
}
