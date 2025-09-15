/// @copyright Los Más Fritos - 2025

#include "FileSystem.hpp"

FileSystem::FileSystem() {
  this->n = 0;
  this->TUnit = DISK_SIZE;
  this->unit = new char[TUnit]; // 1 MB


  this->dir = new directory_t;   // Reserva memoria para el directorio
  this->dir->usedInodes = 0;     // Inicializa inodos usados
  // Guarda la hora en que se crea el directorio
  this->dir->creationTime = time(0);
}

FileSystem::~FileSystem() {
  delete this->unit;
  delete this->dir;
  delete this->fat;
}

int FileSystem::createFile(string name) {
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