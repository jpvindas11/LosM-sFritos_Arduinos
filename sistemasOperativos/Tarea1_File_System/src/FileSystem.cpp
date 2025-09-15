/// @copyright Los Más Fritos - 2025

#include "FileSystem.hpp"
#include <cstring>

FileSystem::FileSystem() {
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
  // Buscar el índice de un inodo libre
  int freeInodeIndex = NO_INDEX_FOUND;

  for (int i = 0; i < TOTAL_I_NODES; ++i) {
    if (!this->inodes[i].isUsed) {
      freeInodeIndex = i;
      break;
    }
  }

  // Dar error si no hay espacio
  if (freeInodeIndex == NO_INDEX_FOUND) { 
    cout << "Error: No hay inodos libres disponibles" << endl;
    return NO_INDEX_FOUND; 
  }

  // Buscar bloque libre
  int freeBlockIndex = NO_INDEX_FOUND;

  for (int i = 0; i < BLOCK_TOTAL; ++i) {
    if (this->fat[i] == FREE_BLOCK) {
      freeBlockIndex = i;
      break;
    }
  }

  // Dar error si no hay espacio
  if (freeBlockIndex == NO_INDEX_FOUND) { 
    cout << "Error: No hay bloques libres disponibles" << endl;
    return NO_INDEX_FOUND; 
  }


  // Crear archivo si hay inodos y bloques disponibles
  iNode_t* inodo = &this->inodes[freeInodeIndex];
  inodo->isUsed = true;
  inodo->creationTime = time(0);
  inodo->user = 1000;
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

  this->fat[freeBlockIndex] = OCUPIED_BLOCK;

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

int FileSystem::search(string filename) {
  for (size_t i = 0; i < dir->usedInodes; ++i) {
    if (dir->files[i].isUsed &&
        filename == string(dir->files[i].fileName)) {
      return i;
    }
  }
  return EXIT_FAILURE;
}

void FileSystem::printDirectory() {}
void FileSystem::printUnidad() {}