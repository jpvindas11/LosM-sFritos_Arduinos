/// @copyright Los Más Fritos - 2025

#ifndef FILE_SYSTEM_2025B
#define FILE_SYSTEM_2025B

#include <array>
#include <cstdlib>
#include <iostream>
#include <string>

#include "FileSysError.hpp"
#include "Structures.hpp"

using namespace std;

/// @brief ...
class FileSystem {

 private:
  int userID;
  string memoryDisk;
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
  void changeMemoryDisk(string newDisk);
  
 private:  
  int open(string file);
  int close(string file);
  bool exist(string file);
  int isOpen(string file);

  int searchEmptyNode();
  int searchFreeBlock();

  void writeBlockToDisk(int blockIndex);
  void readBlockFromDisk(int blockIndex);

  void writeNodeToDisk(int nodeIndex);
  void readNodeFromDisk(int nodeIndex);

  void writeDirToDisk();
  void readDirFromDisk();

  void writeFatToDisk();
  void readFatFromDisk();

  void saveToDisk();
  void loadFromDisk();
};

#endif  // FILE_SYSTEM_2025B
