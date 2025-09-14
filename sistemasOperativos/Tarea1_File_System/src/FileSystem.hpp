/// @copyright Los Más Fritos - 2025

#ifndef FILE_SYSTEM_2025B
#define FILE_SYSTEM_2025B

#include <array>
#include <cstdlib>
#include <iostream>
#include <string>

#include "Structures.hpp"

using namespace std;

// Recuerdo que había un struct...
struct file {
  // Permisos para usuarios
  uint8_t user;
  uint8_t group;
  uint8_t all;
  // Metadatos
  string autor;
  string name;
  string path;
  size_t size;
  // Datos
  char* data;
};

/// @brief ...
class FileSystem {

 private:
  char* unit;
  directory* directory;
  int* fat;
  int n;
  int TDirectory;
  int TUnit;

 // Metadatos(?)
 protected:
  array<uintptr_t, BLOCK_SIZE> index;

 // Funciones
 public:
  FileSystem();

  ~FileSystem();
  
  int search(string file);
  
  int read(string file, int cursor, size_t size);
  
  int write(string file, int cursor, size_t size);

  int rename(string file, string name);

  int createFile(string name);

  int deleteFile(string file);

  void printDirectory();

  void printUnidad();
  
 private:  
  int open(string file);
  
  int close(string file);

  int exist(string file);

  int isOpen(string file);

};

#endif  // FILE_SYSTEM_2025B
