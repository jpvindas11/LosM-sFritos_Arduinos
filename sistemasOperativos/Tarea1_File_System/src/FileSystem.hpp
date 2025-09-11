/// @copyright ...

#ifndef FILE_SYSTEM_2025B
#define FILE_SYSTEM_2025B

#include <array>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

// Recuerdo que había un struct...
struct foo {};

/// @brief ...
class FileSystem {

 // Metadatos(?)
 protected:

  array<uintptr_t, 100> index;

 // Funciones
 public:

  FileSystem();

  ~FileSystem();
  
  int buscar(string archivo);
  
  int escribir(string archivo, int cursor, size_t tamaño);
  
  int leer(string archivo, int cursor, size_t tamaño);

  int renombrar(string archivo, string nombre_nuevo);
  
  int eliminar(string archivo);
  
  private:
  
  int abrir(string archivo);
  
  int cerrar(string archivo);

  int existe(string archivo);

  int estaAbierto(string archivo);
};

#endif  // FILE_SYSTEM_2025B
