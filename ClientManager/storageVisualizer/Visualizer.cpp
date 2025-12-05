#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <memory>
#include <iostream>
#include <limits>
#include <string>

#include "Visualizer.hpp"

Visualizer::Visualizer(){
  if(std::string(DISK_PATH).empty()) {
    throw std::runtime_error("Error: No se definió un disco para el visualizador.");
  }
  this->StorageDisk.mount(DISK_PATH);
  this->startMenu = true;
}

Visualizer::~Visualizer(){
  this->StorageDisk.unmount();
}


void Visualizer:: startInteraction() {
  std::cout<<"=== Visualizador de Storage ===\n";
  while(true) {
    try{
      this->waitForEnter();
      this->refreshDisk();
      std::string fileName = this->userMenu();
      std::string copyName = fileName;
      std::replace(copyName.begin(), copyName.end(), '/', '_');
      this->exportFile(fileName, copyName);
    } catch (const std::runtime_error& e) {
      std::cout <<e.what()<<std::endl;
      break;
    }
  }
}

std::string Visualizer::userMenu(){

  std::vector<std::string> files = this->StorageDisk.listFiles();

    if (files.empty()) {
        std::cerr << "No hay archivos en el sistema.\n";
        return "";
    }

    std::cout << "\n=== Archivos en el FileSystem ===\n";
    for (size_t i = 0; i < files.size(); i++) {
        std::cout << " [" << i << "] " << files[i] << "\n";
    }

    std::cout << "\nIngrese el índice del archivo a exportar: ";

    int index;
    while (true)
    {
      std::cin >> index;

      // check if input was not an integer
      if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Entrada inválida. Ingrese un número entre 0 y "
                  << (files.size() - 1) << ": ";
        continue;
      }

      // Check that the number is not out of range
      if (index < 0 || static_cast<size_t>(index) >= files.size()) {
          std::cout << "Índice fuera de rango. Intente de nuevo (0–"
                    << (files.size() - 1) << "): ";
          continue;
      }

      break;
    }
    // return the file name
    return files[index];
}

bool Visualizer::exportFile(const std::string& fileName,
                            const std::string& hostOutPath){
  // Get the file size
    uint32_t fileSize = this->StorageDisk.getFileSize(fileName);
    if (fileSize == 0) {
        std::cerr << "Archivo vacío o no encontrado en el FS: " << fileName << std::endl;
        return false;
    }

    // Open the file in the specified directory
    std::ofstream out(hostOutPath, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "No se pudo crear archivo de salida: " << hostOutPath << std::endl;
        return false;
    }

    // Read buffer
    const uint32_t block = BLOCK_SIZE;
    std::unique_ptr<char[]> buffer(new char[BLOCK_SIZE]);

    uint32_t cursor = 0;

    while (cursor < fileSize)
    {
        uint32_t toRead = std::min(block, fileSize - cursor);
        uint32_t readBytes = toRead;

        // read the file by chunks of size block
        if (!this->StorageDisk.readFile(fileName, cursor, buffer.get(), readBytes)) {
            std::cerr << "Error leyendo desde el FS (offset = " << cursor << ")" << std::endl;
            return false;
        }

        if (readBytes == 0) {
            std::cerr << "Error: readFile devolvió 0 bytes (posible inconsistencia FS)" << std::endl;
            return false;
        }

        // Write from disk to file
        out.write(buffer.get(), readBytes);
        if (!out) {
            std::cerr << "Error escribiendo al archivo del host." << std::endl;
            return false;
        }

        cursor += readBytes;
    }

    out.close();
    return true;
}


void Visualizer ::waitForEnter() {
  std::string input;

  while (true) {
    // Solo limpiar si el último input dejó caracteres pendientes
    if (!this->startMenu) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cout << std::endl
              << "Presione ENTER para continuar o -1 para finalizar: " 
              << std::flush;
    std::getline(std::cin, input);

    // if line empty then the user just typed enter
    if (input.empty()) {
      this->startMenu = false;
      break;
    }
    // termination signal
    else if (input == "-1") {
      throw std::runtime_error("Cerrando visualizador de storage.");
    }
  }
}

void Visualizer::refreshDisk(){
  this->StorageDisk.unmount();
  this->StorageDisk.mount(DISK_PATH);
}