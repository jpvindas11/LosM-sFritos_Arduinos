#include <iostream>
#include "FileSystem.hpp"

int main() {
  FileSystem *fs = new FileSystem();

  fs->createFile("archivo1.txt");
  fs->createFile("archivo2.txt");
  fs->read("archivo1.txt", 0, 100, nullptr);

  fs->printDirectory();
  fs->printUnidad();

  return 0;
}