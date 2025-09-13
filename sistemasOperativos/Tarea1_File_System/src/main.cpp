#include <iostream>
#include "FileSystem.hpp"

int main() {
  FileSystem *fs = new FileSystem();

  fs->createFile("archivo1.txt");

  fs->printDirectory();
  fs->printUnidad();

  return 0;
}