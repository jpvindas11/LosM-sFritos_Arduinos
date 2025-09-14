/// @copyright Los Más Fritos - 2025

#include "FileSystem.hpp"

FileSystem::FileSystem() {
  this->n = 0;
  this->unit = new char[TUnit]; // 1 MB

  // Guarda la hora en que se crea el directorio
  this->directory->creationTime = time(0);
}

FileSystem::~FileSystem() {
  delete this->unit;
  delete this->directory;
  delete this->fat;
}

int FileSystem::createFile(string name) {
  return EXIT_SUCCESS;
}

int FileSystem::search(string filename) {
  for (size_t i = 0; i < directory->usedInodes; ++i) {
    if (directory->files[i].isUsed &&
        filename == string(directory->files[i].fileName)) {
      return i;
    }
  }
  return EXIT_FAILURE;
}
