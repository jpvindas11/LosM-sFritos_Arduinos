/// @copyright Los Más Fritos - 2025

// Casos de prueba generados

#include "FileSystem.hpp"
#include <iostream>
#include <iomanip>
#include <ctime>

void FileSystem::inspectDisk() {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return;
    }
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "  INFORMACIÓN DEL SISTEMA DE ARCHIVOS" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    // Información del superbloque
    std::cout << "\n[SUPERBLOQUE]" << std::endl;
    std::cout << "  Magic Number: 0x" << std::hex << superBlock.magicNumber << std::dec << std::endl;
    std::cout << "  Total Bloques: " << superBlock.totalBlocks << std::endl;
    std::cout << "  Bloques Libres: " << superBlock.freeBlocks << std::endl;
    std::cout << "  Total Inodos: " << superBlock.totalInodes << std::endl;
    std::cout << "  Inodos Libres: " << superBlock.freeInodes << std::endl;
    
    // Información de ubicación
    std::cout << "\n[ESTRUCTURA DEL DISCO]" << std::endl;
    std::cout << "  Bitmap Inodos: Bloque " << superBlock.inodeBitmapStart 
              << " (" << superBlock.inodeBitmapBlocks << " bloques)" << std::endl;
    std::cout << "  Bitmap Bloques: Bloque " << superBlock.blockBitmapStart 
              << " (" << superBlock.blockBitmapBlocks << " bloques)" << std::endl;
    std::cout << "  Tabla Inodos: Bloque " << superBlock.inodeTableStart 
              << " (" << superBlock.inodeTableBlocks << " bloques)" << std::endl;
    std::cout << "  Directorio: Bloque " << superBlock.fileDirectoryStart 
              << " (" << superBlock.fileDirectoryBlocks << " bloques)" << std::endl;
    std::cout << "  Bloques Datos: Bloque " << superBlock.dataBlocksStart 
              << " (" << superBlock.totalDataBlocks << " bloques)" << std::endl;
    
    // Estadísticas de uso
    float blockUsagePercent = 100.0f * (superBlock.totalBlocks - superBlock.freeBlocks) / superBlock.totalBlocks;
    float inodeUsagePercent = 100.0f * (superBlock.totalInodes - superBlock.freeInodes) / superBlock.totalInodes;
    
    std::cout << "\n[ESTADÍSTICAS DE USO]" << std::endl;
    std::cout << "  Bloques usados: " << (superBlock.totalBlocks - superBlock.freeBlocks) 
              << " / " << superBlock.totalBlocks 
              << " (" << std::fixed << std::setprecision(2) << blockUsagePercent << "%)" << std::endl;
    std::cout << "  Inodos usados: " << (superBlock.totalInodes - superBlock.freeInodes) 
              << " / " << superBlock.totalInodes 
              << " (" << std::fixed << std::setprecision(2) << inodeUsagePercent << "%)" << std::endl;
    
    // Listar archivos
    std::cout << "\n[ARCHIVOS EN EL SISTEMA]" << std::endl;
    uint32_t fileCount = 0;
    for (uint32_t i = 0; i < MAX_FILES; i++) {
        if (directory.entries[i].iNodePointer != INODE_FREE_SLOT) {
            fileCount++;
            std::cout << "  " << fileCount << ". " << directory.entries[i].fileName 
                      << " (inodo " << directory.entries[i].iNodePointer << ")" << std::endl;
        }
    }
    
    if (fileCount == 0) {
        std::cout << "  (No hay archivos)" << std::endl;
    }
    
    std::cout << std::string(80, '=') << std::endl;
}

void FileSystem::printFileDetails(const std::string& fileName) {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return;
    }
    
    int64_t fileIndex = findFileInDirectory(fileName);
    if (fileIndex == -1) {
        std::cerr << "Archivo no encontrado: " << fileName << std::endl;
        return;
    }
    
    uint16_t inodeNum = directory.entries[fileIndex].iNodePointer;
    
    iNode node;
    if (!readInode(inodeNum, &node)) {
        std::cerr << "Error al leer inodo" << std::endl;
        return;
    }
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "  DETALLES DEL ARCHIVO: " << fileName << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    // Información básica
    std::cout << "\n[INFORMACIÓN BÁSICA]" << std::endl;
    std::cout << "  Nombre: " << fileName << std::endl;
    std::cout << "  Inodo: " << inodeNum << std::endl;
    std::cout << "  Tamaño: " << node.size << " bytes" << std::endl;
    std::cout << "  Permisos: 0" << std::oct << node.permissions << std::dec << std::endl;
    std::cout << "  User ID: " << node.userId << std::endl;
    std::cout << "  Group ID: " << node.groupId << std::endl;
    
    // Timestamps
    std::cout << "\n[MARCAS DE TIEMPO]" << std::endl;
    char timeBuffer[80];
    time_t ct = static_cast<time_t>(node.creationTime);
    time_t mt = static_cast<time_t>(node.lastModifiedTime);
    time_t at = static_cast<time_t>(node.lastAccessTime);
    
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", localtime(&ct));
    std::cout << "  Creación: " << timeBuffer << std::endl;
    
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", localtime(&mt));
    std::cout << "  Modificación: " << timeBuffer << std::endl;
    
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", localtime(&at));
    std::cout << "  Último acceso: " << timeBuffer << std::endl;
    
    // Bloques
    uint32_t blocksNeeded = BLOCKS_NEEDED(node.size);
    std::cout << "\n[BLOQUES]" << std::endl;
    std::cout << "  Bloques necesarios: " << blocksNeeded << std::endl;
    
    // Bloques directos
    uint32_t directBlocks = std::min(blocksNeeded, (uint32_t)TOTAL_DIRECT_POINTERS);
    if (directBlocks > 0) {
        std::cout << "  Bloques directos (" << directBlocks << "):" << std::endl;
        for (uint32_t i = 0; i < directBlocks; i++) {
            std::cout << "    [" << i << "] = " << node.directBlocks[i] << std::endl;
        }
    }
    
    // Indirección simple
    if (blocksNeeded > MAX_DIRECT_BLOCKS) {
uint32_t singleIndirectBlocks = std::min<uint32_t>(blocksNeeded - MAX_DIRECT_BLOCKS,
                                                     MAX_SINGLE_INDIRECT_BLOCKS - MAX_DIRECT_BLOCKS);
        std::cout << "  Indirección simple (" << singleIndirectBlocks << " bloques):" << std::endl;
        std::cout << "    Puntero: " << node.singleIndirectPointer << std::endl;
    }
    
    // Indirección doble
    if (blocksNeeded > MAX_SINGLE_INDIRECT_BLOCKS) {
        uint32_t doubleIndirectBlocks = blocksNeeded - MAX_SINGLE_INDIRECT_BLOCKS;
        std::cout << "  Indirección doble (" << doubleIndirectBlocks << " bloques):" << std::endl;
        std::cout << "    Puntero: " << node.doubleIndirectPointer << std::endl;
    }
    
    std::cout << std::string(80, '=') << std::endl;
}

void FileSystem::printAllFiles() {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return;
    }
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "  LISTA DE ARCHIVOS" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    std::cout << std::left << std::setw(30) << "Nombre" 
              << std::right << std::setw(10) << "Inodo"
              << std::setw(15) << "Tamaño (B)"
              << std::setw(10) << "Permisos" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    uint32_t fileCount = 0;
    for (uint32_t i = 0; i < MAX_FILES; i++) {
        if (directory.entries[i].iNodePointer != INODE_FREE_SLOT) {
            fileCount++;
            
            iNode node;
            if (!readInode(directory.entries[i].iNodePointer, &node)) {
                continue;
            }
            
            std::cout << std::left << std::setw(30) << directory.entries[i].fileName
                      << std::right << std::setw(10) << directory.entries[i].iNodePointer
                      << std::setw(15) << node.size
                      << std::setw(10) << std::oct << node.permissions << std::dec << std::endl;
        }
    }
    
    if (fileCount == 0) {
        std::cout << "  (No hay archivos en el sistema)" << std::endl;
    }
    
    std::cout << std::string(80, '=') << std::endl;
    std::cout << "Total de archivos: " << fileCount << std::endl;
}

void FileSystem::printInodeDetails(uint16_t inodeNum) {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return;
    }
    
    if (inodeNum >= TOTAL_I_NODES) {
        std::cerr << "Número de inodo fuera de rango" << std::endl;
        return;
    }
    
    iNode node;
    if (!readInode(inodeNum, &node)) {
        std::cerr << "Error al leer inodo" << std::endl;
        return;
    }
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "  DETALLES DEL INODO #" << inodeNum << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    std::cout << "\n[METADATOS]" << std::endl;
    std::cout << "  Tamaño: " << node.size << " bytes" << std::endl;
    std::cout << "  Permisos: 0" << std::oct << node.permissions << std::dec << std::endl;
    std::cout << "  User ID: " << node.userId << std::endl;
    std::cout << "  Group ID: " << node.groupId << std::endl;
    
    // Timestamps
    std::cout << "\n[TIMESTAMPS]" << std::endl;
    std::cout << "  Creación: " << node.creationTime << std::endl;
    std::cout << "  Modificación: " << node.lastModifiedTime << std::endl;
    std::cout << "  Acceso: " << node.lastAccessTime << std::endl;
    
    // Punteros directos
    std::cout << "\n[BLOQUES DIRECTOS]" << std::endl;
    for (int i = 0; i < TOTAL_DIRECT_POINTERS; i++) {
        if (node.directBlocks[i] != BLOCK_FREE_SLOT) {
            std::cout << "  [" << i << "] = " << node.directBlocks[i] << std::endl;
        }
    }
    
    // Punteros indirectos
    std::cout << "\n[PUNTEROS INDIRECTOS]" << std::endl;
    std::cout << "  Simple: " << node.singleIndirectPointer << std::endl;
    std::cout << "  Doble: " << node.doubleIndirectPointer << std::endl;
    
    std::cout << std::string(80, '=') << std::endl;
}

void FileSystem::printBlockContents(uint32_t blockNum, bool asText) {
    if (!isMounted) {
        std::cerr << "Disco no montado" << std::endl;
        return;
    }
    
    if (blockNum >= TOTAL_BLOCK) {
        std::cerr << "Número de bloque fuera de rango" << std::endl;
        return;
    }
    
    char buffer[BLOCK_SIZE];
    if (!readBlock(blockNum, buffer)) {
        std::cerr << "Error al leer bloque" << std::endl;
        return;
    }
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "  CONTENIDO DEL BLOQUE #" << blockNum << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    if (asText) {
        std::cout << "\n[COMO TEXTO]" << std::endl;
        for (int i = 0; i < BLOCK_SIZE; i++) {
            char c = buffer[i];
            if (c >= 32 && c <= 126) {
                std::cout << c;
            } else if (c == '\n') {
                std::cout << "\\n";
            } else if (c == '\t') {
                std::cout << "\\t";
            } else if (c == 0) {
                std::cout << ".";
            } else {
                std::cout << "?";
            }
            
            if ((i + 1) % 64 == 0) {
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
    } else {
        std::cout << "\n[COMO HEXADECIMAL]" << std::endl;
        for (int i = 0; i < BLOCK_SIZE; i++) {
            if (i % 16 == 0) {
                std::cout << std::setw(4) << std::setfill('0') << std::hex << i << ": ";
            }
            
            std::cout << std::setw(2) << std::setfill('0') << std::hex 
                      << (static_cast<int>(buffer[i]) & 0xFF) << " ";
            
            if ((i + 1) % 16 == 0) {
                std::cout << " ";
                for (int j = i - 15; j <= i; j++) {
                    char c = buffer[j];
                    if (c >= 32 && c <= 126) {
                        std::cout << c;
                    } else {
                        std::cout << ".";
                    }
                }
                std::cout << std::endl;
            }
        }
        std::cout << std::dec << std::endl;
    }
    
    std::cout << std::string(80, '=') << std::endl;
}