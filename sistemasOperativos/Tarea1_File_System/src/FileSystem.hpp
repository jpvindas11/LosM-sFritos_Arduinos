/// @copyright Los Más Fritos - 2025

#ifndef FILE_SYSTEM_2025B
#define FILE_SYSTEM_2025B

#include <array>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

#include "FileSysError.hpp"
#include "Structures.hpp"

using namespace std;

/// @brief ...
class FileSystem {

 private:
  int userID;
  char* unit;
  directory* dir;
  iNode_t* inodes;
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

  int createFile(string name);
  int deleteFile(string file);
  
  int search(string filename);
  int read(string file, int cursor, size_t size, char* buffer);
  int write(string file, int cursor, size_t size, const char* buffer);
  int rename(string file, string name);

  void printDirectory();
  void printUnidad();

  void changeUserID(int newID);
  
  // Métodos para serialización
  int saveToDisk(const string& filename);
  int loadFromDisk(const string& filename);
  
 private:  
  int open(string file);
  int close(string file);
  bool exist(string file);
  int isOpen(string file);

  int searchEmptyNode();
  int searchFreeBlock();
  
  // Métodos privados para serialización
  void writeHeader(ofstream& file);
  void readHeader(ifstream& file);
  uint32_t calculateUsedBlocks();

};

#endif  // FILE_SYSTEM_2025B
