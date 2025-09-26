/// @copyright Los Más Fritos - 2025

#include <iostream>
#include "FileSystem.hpp"

int main() {
  FileSystem *fs = new FileSystem();

  // 1. Crear algunos archivos y escribir contenido
  fs->createFile("archivo1.txt");
  fs->write("archivo1.txt", 0, 100, "Contenido del archivo 1 de prueba. Probando escritura y lectura.");

  fs->createFile("archivo2.txt");
  fs->write("archivo2.txt", 0, 50, "Este es el contenido del segundo archivo.");
  
  fs->createFile("datos.log");
  fs->write("datos.log", 0, 30, "Log de sistema - entrada 001");

  fs->printDirectory();

  if (fs->saveToDisk("filesystem.bin") == 0) {
    std::cout << "Sistema guardado exitosamente!" << std::endl;
  } else {
    std::cout << "Error al guardar el sistema!" << std::endl;
  }

  char buffer[100] = {0};
  fs->read("archivo1.txt", 0, 100, buffer);
  std::cout << "\nContenido leído: " << buffer << std::endl;

  FileSystem *fs2 = new FileSystem();
  
  std::cout << "Directorio del nuevo sistema:" << std::endl;
  fs2->printDirectory();
  
  if (fs2->loadFromDisk("filesystem.bin") == 0) {
    std::cout << "\nSistema cargado exitosamente!" << std::endl;
    
   std::cout << "\nNuevo directorio:" << std::endl;
    fs2->deleteFile("datos.log");
    fs2->createFile("hola.log");
    fs2->createFile("hola1.log");
    fs2->createFile("hola2.log");
    fs2->printDirectory();
    
    char buffer2[100] = {0};
    fs2->read("archivo1.txt", 0, 100, buffer2);
    std::cout << "\nContenido después de cargar: " << buffer2 << std::endl;
    
    char buffer3[50] = {0};
    fs2->read("archivo2.txt", 0, 50, buffer3);
    std::cout << "Archivo 2 después de cargar: " << buffer3 << std::endl;
  } else {
    std::cout << "Error al cargar el sistema!" << std::endl;
  }
  if (fs2->saveToDisk("filesystem.bin") == 0) {
    std::cout << "Sistema guardado exitosamente!" << std::endl;
  } else {
    std::cout << "Error al guardar el sistema!" << std::endl;
  }

  delete fs;
  delete fs2;

  return 0;
}
