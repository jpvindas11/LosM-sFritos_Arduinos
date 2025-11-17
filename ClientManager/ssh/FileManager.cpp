#include "FileManager.hpp"

std::vector<std::string> FileManager::readFile(const std::string& filePath) {
    std::vector<std::string> lines;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return lines; // Retorna vector vacío si no se puede abrir el archivo
    }
    std::string line;
    char delimiter = ',';
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string item;
        while (std::getline(ss, item, delimiter)) {
            lines.push_back(item);
        }
    }
    file.close();
    return lines;
}