/// @copyright Los Más Fritos - 2025

#include "FileSystem.hpp"
#include <cstring>

FileSystem::FileSystem() {
  // ID de usuario por defecto
  this->userID = 1000;
  this->n = 0;
  this->TUnit = DISK_SIZE;
  this->unit = new char[TUnit]; 

  memset(this->unit, 0, TUnit);

  this->dir = new directory_t;   // Reserva memoria para el directorio
  this->dir->usedInodes = 0;     // Inicializa inodos usados
  // Guarda la hora en que se crea el directorio
  this->dir->creationTime = time(0);
  strcpy(this->dir->dirName, "root");
  
  // Inicializar la tabla FAT (File Allocation Table)
  this->fat = new int[BLOCK_TOTAL];
  for (int i = 0; i < BLOCK_TOTAL; i++) {
    this->fat[i] = FREE_BLOCK;
  }
  
  // Inicializar los inodos
  this->inodes = new iNode_t[TOTAL_I_NODES];
  for (int i = 0; i < TOTAL_I_NODES; i++) {
    this->inodes[i].isUsed = false;
  }
}

FileSystem::~FileSystem() {
  delete[] this->unit;
  delete this->dir;
  delete[] this->fat;
  delete[] this->inodes;
}

int FileSystem::createFile(string name) {
  try {
    // Se asegura que no exista un archivo con el mismo nombre
    if (!exist(name)) {
      int freeInodeIndex = searchEmptyNode();
      int freeBlockIndex = searchFreeBlock();
      // Crear archivo si hay inodos y bloques disponibles
      iNode_t* inodo = &this->inodes[freeInodeIndex];
      inodo->isUsed = true;
      inodo->creationTime = time(0);
      inodo->user = this->userID;
      inodo->groupId = 1000;
      inodo->permissions = 0644; // rw- r-- r--
      inodo->size = 0;
      inodo->lastUsedBlock = 0;
      strcpy(inodo->path, "/");
    
      // Bloques directos
      for (int i = 0; i < TOTAL_POINTERS; ++i) {
        inodo->directBlocks[i] = 0;
      }
  
      inodo->directBlocks[0] = freeBlockIndex; // Primer bloque
  
      // Indirección
      // Solo se usa al necesitar más bloques fuera de los directos
      inodo->singleIndirect.isUsed = false;
      inodo->singleIndirect.usedDataPtr = 0;
      inodo->doubleIndirect.isUsed = false;
      inodo->doubleIndirect.usedIndex = 0;
  
      this->fat[freeBlockIndex] = ERR_OCCUPIED_BLOCK;
  
      // Agregar entrada al directorio 
      fileEntry_t* newEntry = &this->dir->files[this->dir->usedInodes];
      strncpy(newEntry->fileName, name.c_str(), NAME_MAX - 1);
      newEntry->fileName[NAME_MAX - 1] = '\0';
      newEntry->iNodeIndex = freeInodeIndex;
      newEntry->isUsed = true;
  
      this->dir->usedInodes++; // Inodos totales
  
      cout << "Archivo '" << name << "' creado correctamente" << endl;
      cout << "Inodo asignado: " << freeInodeIndex << endl;
      cout << "Bloque asignado: " << freeBlockIndex << endl;
    
      return EXIT_SUCCESS;
    }
  } catch (const FileSysError& err) {
    cerr << "Error al crear archivo: " << err.what() << endl;
    return err.code();
  }
}

int FileSystem::deleteFile(string file) {
  try {
    if (exist(file)) {
      fileEntry* target = &this->dir->files[search(file)];
      // marca el nodo como libre
      this->inodes[target->iNodeIndex].isUsed = false;
      target->iNodeIndex = FREE_INDEX;
      // marca el espacio del directorio como libre
      target->isUsed = false;
      // Elimina todos los datos del archivo(?)
      // clearFileEntry(target)
      return EXIT_SUCCESS;
    }
  } catch (const FileSysError& err) {
    cerr << "Error al eliminar archivo: " << err.what() << endl;
    return err.code();
  }
}

int FileSystem::search(string filename) {
  for (size_t i = 0; i < dir->usedInodes; ++i) {
    if (dir->files[i].isUsed &&
        filename == dir->files[i].fileName) {
      return i;
    }
  }
  throw FileSysError(ERR_NO_FILE_FOUND,
    "No se encontró el archivo llamado " + filename);
  return ERR_NO_FILE_FOUND;
}

int FileSystem::read(string file, int cursor, size_t size, char* buffer) {
  int fileIndex = search(file);
  if (fileIndex == ERR_NO_INDEX_FOUND) {
    cerr << "Error: El archivo no existe" << endl;
    return ERR_NO_INDEX_FOUND;
  }

  iNode_t* inode = &this->inodes[fileIndex];
  if (!inode->isUsed) {
    cerr << "Error: El i nodo no está en uso" << endl;
    return ERR_NO_FILE_FOUND;
  }

  if (cursor > (int)inode->size) {
    cerr << "Error: El cursor se encuentra fuera del rango del archivo" << endl;
    return -1;
  }

  size_t bytesToRead = min(size, static_cast<size_t>(inode->size - cursor));
  buffer = new char[bytesToRead];
  size_t bytesRead = 0;

  const size_t maxBlocks = TOTAL_POINTERS + TOTAL_POINTERS + TOTAL_POINTERS * TOTAL_POINTERS;

  while (bytesRead < bytesToRead) {
    int blockIndex = (cursor + bytesRead) / BLOCK_SIZE;
    int blockOffset = (cursor + bytesRead) % BLOCK_SIZE;

    block_size_t dataBlockNum = 0;

    if (blockIndex >= maxBlocks) {
      cerr << "Error: El archivo es demasiado grande para leer" << endl;
      break;
    }

    if (blockIndex < TOTAL_POINTERS) {
      dataBlockNum = inode->directBlocks[blockIndex];
    } else if (blockIndex < TOTAL_POINTERS + TOTAL_POINTERS) {
      int idx = blockIndex - TOTAL_POINTERS;
      dataBlockNum = inode->singleIndirect.dataPtr[idx];
    } else {
      int idx = blockIndex - TOTAL_POINTERS - TOTAL_POINTERS;
      int outer = idx / TOTAL_POINTERS;
      int inner = idx % TOTAL_POINTERS;
      dataBlockNum = inode->doubleIndirect.dataIndex[outer].dataPtr[inner];
    }

    if (dataBlockNum == FREE_BLOCK) {
      break;
    }

    dataBlock_t* block = (dataBlock_t*)&unit[dataBlockNum * sizeof(dataBlock_t)];
    size_t canRead = min(bytesToRead - bytesRead, (size_t)BLOCK_SIZE - blockOffset);
    memcpy(buffer + bytesRead, block->data + blockOffset, canRead);

    bytesRead += canRead;
  }
  return bytesRead;
}

int FileSystem::write(string file, int cursor, size_t size, char* buffer) {
  try {
    if (exist(file)) {
      iNode_t* inode = &this->inodes[search(file)];
      if (!inode->isUsed) {
        throw FileSysError(ERR_EMPTY_INODE, "El inodo no esta en uso");
      }
      if (cursor > (int)inode->size) {
        throw FileSysError(ERR_OUT_OF_RANGE,
          "El cursor se encuentra fuera del rango del archivo");
      }
    }
  } catch (const FileSysError& err) {
    cerr << "Error al escribir archivo: " << err.what() << endl;
    return err.code();
  }
}

int FileSystem::rename(string filename, string newname) {
  try {
    if (exist(filename) && !exist(newname)) {
      fileEntry* target = &this->dir->files[search(filename)];
      strncpy(target->fileName, newname.c_str(), NAME_MAX - 1);
      target->fileName[NAME_MAX - 1] = '\0';
      return EXIT_SUCCESS;
    }
  } catch (const FileSysError& err) {
    cerr << "Error al renombrar archivo: " << err.what() << endl;
    return err.code();
  }
}

void FileSystem::printDirectory() {}
void FileSystem::printUnidad() {}

void FileSystem::changeUserID(int newID) {
  this->userID = newID;
}

// Private

int FileSystem::open(string file) {}
int FileSystem::close(string file) {}

int FileSystem::exist(string filename) {
  if (search(filename) == ERR_NO_INDEX_FOUND) {
    return EXIT_SUCCESS;
  }
  throw FileSysError(ERR_OCCUPIED_FILENAME,
    "Ya existe un archivo llamado " + filename);
}


int FileSystem::searchEmptyNode(){
  for (size_t index = 0; index < TOTAL_I_NODES; ++index) {
    if (!this->inodes[index].isUsed) {
      return index;
    }
  }
  throw FileSysError(ERR_NO_FREE_INODES, "No hay inodos libres disponibles");
}

int FileSystem::searchFreeBlock(){
  for (size_t index = 0; index < BLOCK_TOTAL; ++index) {
    if (this->fat[index] == FREE_BLOCK) {
      return index;
    }
  }
  throw FileSysError(ERR_NO_FREE_BLOCKS, "No hay bloques libres disponibles");
}
