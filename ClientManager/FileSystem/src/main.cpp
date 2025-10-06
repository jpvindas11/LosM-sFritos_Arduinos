/// @copyright Los Más Fritos - 2025
#include "FileSystem.hpp"
#include <cstring>
#include <iostream>

int main() {
    FileSystem fs;
    fs.mount("bin/disk.bin");
    fs.createFile("hola.txt");
    
    const char* msg = "Esto es una prueba.";
    uint32_t msgLen = strlen(msg) + 1;
    
    fs.writeFile("hola.txt", msg, msgLen);
    
    char result[20];
    uint32_t size = 20;
    
    fs.readFile("hola.txt", result, size);
    
    std::cout << "Leído: " << result << std::endl;
    std::cout << "Bytes leídos: " << size << std::endl;
    
    fs.unmount();
    return 0;
}