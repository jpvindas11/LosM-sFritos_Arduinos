#include "FileSystem.hpp"
#include <iostream>
#include <string>

int main() {
  // Crear e inicializar el servidor de autenticación
  FileSystem* filesystem = new FileSystem();
  filesystem->createFile("test.txt");
  char buffer[100] = "abc";
  filesystem->write("test.txt", 0, 3, buffer);
  filesystem->printDirectory();
  filesystem->printUnidad();
  char buffer2[100] = "cde";
  filesystem->append("test.txt", 3, buffer2);
  filesystem->printUnidad();
  delete filesystem;
  return 0;
}
