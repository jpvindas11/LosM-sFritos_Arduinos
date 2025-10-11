#include <sstream>
#include "VirtualMemoryManager.hpp"


VirtualMemoryManager::VirtualMemoryManager() {
  this->disk = new FileSystem();
  if (disk->mount("src/FileSystem/disk.bin")) {
    this->initializeManager();
  }
}

VirtualMemoryManager::~VirtualMemoryManager() {
  if (this->disk->isMounted) {
    delete this->disk;
  }
  this->correspondanceTable.clear();
}

void VirtualMemoryManager::initializeManager() {
  for (size_t index = 0; index < FRAME_COUNT; index++) {
    this->pageTable[index] = -1;
    this->timeTable[index] = -1;
  }
  this->time_counter = 0;
  uint32_t file_size = this->disk->getFileSize("correspondenceTable.bin") + 1;
  char tableContent[file_size];
  this->disk->readFile("correspondenceTable.bin", tableContent, file_size);
  std::vector<std::string> pages;
  std::cout<<tableContent<<std::endl;
  this->proccessFileCorrespondenceTable(tableContent, pages);
  this->fillCorrespondanceTable(pages);
} 

void VirtualMemoryManager::proccessFileCorrespondenceTable(char* tableContent, 
                                        std::vector<std::string>& directions) {
  std::string content(tableContent);
  std::stringstream ss(content);
  std::string correspondePair;

  while (std::getline(ss, correspondePair, ':')) {
    if (!correspondePair.empty()) {
      directions.push_back(correspondePair);
    }
  }
}

void VirtualMemoryManager:: fillCorrespondanceTable(
                                              std::vector<std::string>& pages) {
  for (const auto& pair: pages) {
    size_t commaPostion = pair.find(',');
    if (commaPostion != std::string::npos) {
      std::string logicalPage = pair.substr(0, commaPostion);
      std::string physicalPage = pair.substr(commaPostion+1);
      if (!logicalPage.empty() && !physicalPage.empty()) {
        this->correspondanceTable[logicalPage] = physicalPage;
      }
    }
  }
}