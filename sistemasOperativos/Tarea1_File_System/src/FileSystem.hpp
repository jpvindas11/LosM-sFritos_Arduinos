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

/// @brief Sistema de manejo de archivos
class FileSystem {
 protected:  // Metadatos del file system
  /// ID del usuario
  int userID;
  /// Archivo .bin utilizado como disco de memoria
  string memoryDisk;
  /// Unidad de memoria
  char* unit;
  /// Estructura de directorio
  directory* dir;
  /// Estructura de inodos
  iNode_t* inodes;
  /// tabla FAT
  int* fat;
  /// ?
  int n;
  /// Tamaño del directorio
  int TDirectory;
  /// Tamaño de la unidad
  int TUnit;

 public:  // Funciones
  /// Constructor
  FileSystem();
  /// Destructor
  ~FileSystem();
  /// Crea un archivo
  int createFile(string filefilename);
  /// Elimina un archivo
  int deleteFile(string filenamename);
  /// Busca un archivo por su nombre
  int search(string filename);
  /// Lee un archivo
  int read(string filenamename, int cursor, size_t size, char* buffer);
  /// Escribe en un archivo
  int write(string filenamename, int cursor, size_t size, const char* buffer);
  /// Cambia el nombre de un archivo
  int rename(string filenamename, string newnewname);
  /// Imprime todos los archivos en el directorio
  void printDirectory();
  /// Imprime todos los archivos en la unidad
  void printUnidad();
  /// Cambia el ID del usuario
  void changeUserID(int newID);
  /// Cambia el disco de memoria utilizado
  void changeMemoryDisk(string newDisk);

 private:  // Funciones privadas
  /// Abre un archivo
  int open(string filename);
  /// @brief Cierra un archivo
  int close(string filename);
  /// Devuelve si existe un archivo con cierto nombre
  bool exist(string filename);
  /// Devuelve si un archivo esta abierto
  bool isOpen(string filename);
  /// Busca un i nodo vació disponible
  /// Cambia el disco de memoria utilizado
  void changeMemoryDisk(string newDisk);
  // Métodos para serialización
  int saveToDisk(const string& filename);
  int loadFromDisk(const string& filename);

 private:  // Funciones privadas
  /// Abre un archivo
  int open(string filename);
  /// @brief Cierra un archivo
  int close(string filename);
  /// Devuelve si existe un archivo con cierto nombre
  bool exist(string filename);
  /// Devuelve si un archivo esta abierto
  bool isOpen(string filename);
  /// Busca un i nodo vació disponible
  int searchEmptyNode();
  /// Busca un bloque vació disponible
  /// Busca un bloque vació disponible
  int searchFreeBlock();
  /// Guarda un bloque directo a memoria
  void writeBlockToDisk(int blockIndex);
  /// Carga un bloque directo de memoria
  void readBlockFromDisk(int blockIndex);
  /// Guarda un nodo directo a memoria
  void writeNodeToDisk(int nodeIndex);
  /// Carga un nodo directo de memoria
  void readNodeFromDisk(int nodeIndex);
  /// Guarda el directorio en la memoria
  void writeDirToDisk();
  /// Carga el directorio de la memoria
  void readDirFromDisk();
  /// Guarda la tabla FAT en la memoria
  void writeFatToDisk();
  /// Carga la tabla FAT de la memoria
  void readFatFromDisk();
  /// Guarda todos los datos y estructuras en la memoria
  void saveToDisk();
  /// Carga todos los datos y estructuras en la memoria
  void loadFromDisk();
  /// Intenta cargar el archivo de memoria, si no hay crea uno
  int getMemDisk();
  // Métodos privados para serialización
  void writeHeader(ofstream& file);
  void readHeader(ifstream& file);
  uint32_t calculateUsedBlocks();
};

#endif  // FILE_SYSTEM_2025B
