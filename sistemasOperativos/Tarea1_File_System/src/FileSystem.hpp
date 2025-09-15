/// @copyright Los Más Fritos - 2025

#ifndef FILE_SYSTEM_2025B
#define FILE_SYSTEM_2025B

#include <array>
#include <cstdlib>
#include <iostream>
#include <string>

#include "Structures.hpp"

using namespace std;

typedef struct file {
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
} file_t;

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

  /*
  inode_size_t totalInodes;
  blockNum_size_t totalDataBlocks;
  size_t leftoverSpace;
  bool* inodeBitMap;
  inode_size_t freeInodes;
  bool* dataBlocksBitMap;
  blockNum_size_t freeDataBlocks;

  uint8_t* unit;
  iNode_t* firstInode;
  directory_t* rootDir;
  dataBlock_t* firstDataBlock;*/

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
  int read(string file, int cursor, size_t size);
  int write(string file, int cursor, size_t size);
  int rename(string file, string name);

  void printDirectory();
  void printUnidad();

  void changeUserID(int newID);
  
 private:  
  //int allocateStructures();
  int open(string file);
  int close(string file);
  int exist(string file);
  int isOpen(string file);

  int searchEmptyNode();
  int searchFreeBlock();
};

#endif  // FILE_SYSTEM_2025B
