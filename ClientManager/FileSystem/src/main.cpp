// ...existing code...
#include <iostream>
#include <vector>
#include <cstring>
#include "FileSystem.hpp"

int main() {
    FileSystem fs;
    const std::string diskPath = "disk.bin";
    if (!fs.mount(diskPath)) {
        std::cerr << "mount failed\n";
        return 1;
    }

    std::vector<std::string>names = fs.listFiles();
    const uint32_t chunkSize = 512;
    
    for (std::string file : names) {
        std::cout << file << std::endl;
        // Primera lectura: offset 0
        char buf1[chunkSize];
        uint32_t sz1 = chunkSize;
        bool ok1 = fs.readFile(file, 0u, buf1, sz1);
        std::cout << std::string(buf1, sz1) << "\n";
    }

    std::string dis = "Distancia:_1_2025/11/3.log";
    std::string hum = "Humedad:_2_2025/11/3.log";
    std::string uv = "UV:_3_2025/11/3.log";



    /*
    const std::string fname = "test.log";

    const std::string content = "0123456789"; // 10 caracteres
    
    // Crear y escribir el archivo
    if (!fs.fileExists(fname)) {
        if (!fs.createFile(fname)) {
            std::cerr << "createFile failed\n";
            fs.unmount();
            return 1;
        }
    }

    if (!fs.writeFile(fname, content.data(), static_cast<uint32_t>(content.size()))) {
        std::cerr << "writeFile failed\n";
        fs.unmount();
        return 1;
    }
    

    uint32_t fsize = fs.getFileSize(fname);
    std::cout << "File size: " << fsize << " bytes\n";

    // Leer en dos lecturas sucesivas de 5 bytes usando offset
    const uint32_t chunkSize = 5;

    // Primera lectura: offset 0
    char buf1[chunkSize];
    uint32_t sz1 = chunkSize;
    bool ok1 = fs.readFile(fname, 0u, buf1, sz1);
    std::cout << "Read 1 ok=" << ok1 << " bytes=" << sz1
              << " data=\"" << std::string(buf1, sz1) << "\"\n";

    // Segunda lectura: offset 5
    char buf2[chunkSize];
    uint32_t sz2 = chunkSize;
    bool ok2 = fs.readFile(fname, chunkSize, buf2, sz2);
    std::cout << "Read 2 ok=" << ok2 << " bytes=" << sz2
              << " data=\"" << std::string(buf2, sz2) << "\"\n";

    // Intento de lectura más allá del EOF (offset 10)
    char buf3[chunkSize];
    uint32_t sz3 = chunkSize;
    bool ok3 = fs.readFile(fname, static_cast<uint32_t>(fsize), buf3, sz3);
    std::cout << "Read 3 (EOF) ok=" << ok3 << " bytes=" << sz3 << "\n";

    // Comparar resultados esperados
    std::cout << "Expected first chunk: \"" << content.substr(0,5) << "\"\n";
    std::cout << "Expected second chunk: \"" << content.substr(5,5) << "\"\n";

    fs.unmount();
    */
    return 0;
}
// ...existing code...