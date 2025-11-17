#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

class FileManager {

public:
    std::vector<std::string> readFile(const std::string& filePath);
};

#endif // FILEMANAGER_HPP
