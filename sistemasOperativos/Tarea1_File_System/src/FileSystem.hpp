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
  std::array<uintptr_t, 100> index;

 // Funciones
 public:

  FileSystem();

  ~FileSystem();
};

#endif  // FILE_SYSTEM_2025B
