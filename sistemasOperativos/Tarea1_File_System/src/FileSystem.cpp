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
  // Se asegura que no exista un archivo con el mismo nombre
  if (exist(name)) {
    cout << "Error: Este archivo ya existe" << endl;
    return OCCUPIED_FILENAME; 
  }

  // Buscar el índice de un inodo libre
  int freeInodeIndex = searchEmptyNode();
  // Dar error si no hay espacio
  if (freeInodeIndex == NO_INDEX_FOUND) { 
    cout << "Error: No hay inodos libres disponibles" << endl;
    return NO_INDEX_FOUND; 
  }

  // Buscar bloque libre
  int freeBlockIndex = searchFreeBlock();
  // Dar error si no hay espacio
  if (freeBlockIndex == NO_INDEX_FOUND) { 
    cout << "Error: No hay bloques libres disponibles" << endl;
    return NO_INDEX_FOUND; 
  }

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

  this->fat[freeBlockIndex] = OCCUPIED_BLOCK;

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

int FileSystem::deleteFile(string file) {}

int FileSystem::search(string filename) {
  for (size_t i = 0; i < dir->usedInodes; ++i) {
    if (dir->files[i].isUsed &&
        filename == dir->files[i].fileName) {
      return i;
    }
  }
  return NO_INDEX_FOUND;
}

  
int FileSystem::search(string filename) {}
int FileSystem::read(string file, int cursor, size_t size) {}
int FileSystem::write(string file, int cursor, size_t size) {}
int FileSystem::rename(string file, string name) {}

void FileSystem::printDirectory() {}
void FileSystem::printUnidad() {}

void FileSystem::changeUserID(int newID) {
  this->userID = newID;
}

// Private

int FileSystem::open(string file) {}
int FileSystem::close(string file) {}

int FileSystem::exist(string file) {
  return (search(file) != NO_INDEX_FOUND);
}

int FileSystem::isOpen(string file) {}

int FileSystem::searchEmptyNode(){
  for (size_t index = 0; index < TOTAL_I_NODES; ++index) {
    if (!this->inodes[index].isUsed) {
      return index;
    }
  }
  return NO_INDEX_FOUND;
}

int FileSystem::searchFreeBlock(){
  for (size_t index = 0; index < BLOCK_TOTAL; ++index) {
    if (!this->fat[index] == FREE_BLOCK) {
      return index;
    }
  }
  return NO_INDEX_FOUND;
}
