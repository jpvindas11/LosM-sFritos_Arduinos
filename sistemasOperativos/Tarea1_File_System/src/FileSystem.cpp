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
 this->unit = 0;
 //this->allocateStructures();
}

FileSystem::~FileSystem() {
  delete[] this->unit;
  delete this->dir;
  delete[] this->fat;
  delete[] this->inodes;
  /*
  // liberar memoria del disco y de los bitmaps
  free(this->dataBlocksBitMap);
  free(this->inodeBitMap);
  free(this->unit);*/
}

/*
int FileSystem::allocateStructures() {
  // reservar espacio en memoria para el disco
  this->unit = (uint8_t*) std::calloc(DISK_SIZE, sizeof(uint8_t));
  if(this->unit == 0) {
    std::cerr<< "error: no se pudo obtener memoria para el disco"<<std::endl;
    return 0;
  }

  // el primer inodo se encuentra justo después del directorio
  size_t inodeOffset = sizeof(directory_t);
  // el primer bloque de datos se halla justo después del último inodo
  size_t dataOffset = inodeOffset + (sizeof(iNode_t) * (size_t)TOTAL_I_NODES);
  // si se excede la memoria del disco en los demás componentes, no se puede
  // crear bloque para guardar el contenido del archivo
  if (dataOffset >= (size_t)DISK_SIZE) {
    std::cerr<<"error: memoria insuficiente para contenido de los archivos"<<std::endl;
    return 0;
  }
  this->totalDataBlocks = ((size_t)DISK_SIZE - dataOffset) / sizeof(dataBlock_t);
  this->freeDataBlocks = this->totalDataBlocks;
  this->leftoverSpace = ((size_t)DISK_SIZE - dataOffset) % sizeof(dataBlock_t);

  // se realiza el mapeo. reinterpret hará automatico el proceso de arimética de punteros
  // el directorio inicia en la ubicación 0 de la unidad.
  this->rootDir = reinterpret_cast<directory_t*>(this->unit);
  // despues de sizeof(directory_t) bytes se encuentra el primer nodo
  // se guarda esa dirección de memoria
  this->firstInode = reinterpret_cast<iNode_t*>(this->unit + inodeOffset);
  // con el primer bloque es la misma lógica que con inodo, pero considerando
  // toda el área que se brindo solo para inodos
  this->firstDataBlock = reinterpret_cast<dataBlock_t*>(this->unit + dataOffset);

  this->totalInodes = (inode_size_t)TOTAL_I_NODES;
  this->freeInodes = (inode_size_t)TOTAL_I_NODES;

  // bitmaps para mapear las disponibilidad de inodos y bloqyes
  this->inodeBitMap = (bool*) std::calloc((size_t)this->totalInodes, sizeof(bool));
  this->dataBlocksBitMap = (bool*) std::calloc((size_t)this->totalDataBlocks, sizeof(bool));


  Bloque de pruebas para entender el funcionamiento

  this->firstInode[0].creationTime = 0;
  this->firstInode[1].isUsed = true;
  this->firstInode[2].user = 100;

  std::strcpy(this->rootDir->dirName,"u:LUIS/Hola/");
  std::cout<<this->rootDir->dirName<<std::endl;
  std::strcpy(this->rootDir->files[0].fileName,"Hola.txt");
  std::cout<<this->rootDir->files[0].fileName<<std::endl;
  this->rootDir->usedInodes++;
  std::cout<<this->rootDir->usedInodes<<std::endl;

  std::cout<<this->firstInode[0].creationTime<<std::endl;
  std::cout<<this->firstInode[1].isUsed<<std::endl;
  std::cout<<this->firstInode[2].user<<std::endl;

  this->firstDataBlock[0].data[0] = 'A';
  this->firstDataBlock[0].data[1] = 'B';
  this->firstDataBlock[0].data[2] = 'C';
  std::snprintf(&(this->firstDataBlock[0].data[3]), 5, "DEFG");
  this->firstDataBlock[0].offset = 7;
  std::cout<<this->firstDataBlock[0].data<<std::endl;
  std::cout<<this->firstDataBlock[0].offset<<std::endl;

  this->firstDataBlock[12].data[0] = '*';
  this->firstDataBlock[12].data[1] = '2';
  this->firstDataBlock[12].data[2] = '3';
  std::snprintf(&(this->firstDataBlock[12].data[3]), 6, "mkLAQ");
  this->firstDataBlock[12].offset = 8;
  std::cout<<this->firstDataBlock[12].data<<std::endl;
  std::cout<<this->firstDataBlock[12].offset<<std::endl;

  std::cout<<this->firstDataBlock[2].data<<std::endl;
  std::cout<<this->firstDataBlock[2].offset<<std::endl; 

  return 1;
} 
*/

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
