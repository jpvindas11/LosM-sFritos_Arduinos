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

int FileSystem::search(string file) {
  return EXIT_SUCCESS;
}

