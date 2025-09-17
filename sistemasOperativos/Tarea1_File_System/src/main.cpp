#include <iostream>
#include "FileSystem.hpp"

int main() {
  FileSystem *fs = new FileSystem();

  fs->createFile("archivo1.txt");
  fs->write("archivo1.txt", 0, 100, "Contenido del archivo 1 de prueba. Probando escritura y lectura.");
  char buffer[100] = {0};
  fs->read("archivo1.txt", 0, 100, buffer);
  fs->createFile("archivo2.txt");

  std::cout << buffer << endl;

  fs->printDirectory();
  fs->printUnidad();
  return 0;
}