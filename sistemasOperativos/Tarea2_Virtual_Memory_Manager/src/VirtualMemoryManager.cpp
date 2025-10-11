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
  this->timeCounter = 0;
  this->pageFaults = 0;
  this->tableHits = 0;
  this->framesContent.resize(FRAME_COUNT);
  uint32_t file_size = this->disk->getFileSize("correspondenceTable.bin") + 1;
  char tableContent[file_size];
  this->disk->readFile("correspondenceTable.bin", tableContent, file_size);
  std::vector<std::string> pages;
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

void VirtualMemoryManager::fillCorrespondanceTable(
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

int64_t VirtualMemoryManager::getFreePage() {
  for (int64_t index = 0; index < FRAME_COUNT; index++){
    if (this->pageTable[index] == -1) {
      return index;
    }
  }
  return -1;
}

bool VirtualMemoryManager::checkPageInTable(int64_t page) {
  for (int64_t index = 0; index < FRAME_COUNT; index++){
    if (this->pageTable[index] == page) {
      return true;
    }
  }
  return false;
}

int64_t VirtualMemoryManager::getPhysicalPage(std::string& logicalPage) {
  for (const auto& pair: this->correspondanceTable) {
    if (pair.first == logicalPage) {
      return std::stoll(pair.second);
    }
  }
  return -1;
}

int VirtualMemoryManager::allocatePage(std::string& requestedPage,
                                                    int64_t storingFrameIndex) {
  int64_t physicalPage = this->getPhysicalPage(requestedPage);
  if (physicalPage == -1) {
    return EXIT_FAILURE;
  }
  try {
    int64_t numericPage = std::stoll(requestedPage);
    this->pageTable[storingFrameIndex] = numericPage;
    this->timeTable[storingFrameIndex] = this->timeCounter;
    this->timeCounter++;
    this->disk->readBlock(
                          (uint64_t)physicalPage, 
                          static_cast<void*>
                          (this->framesContent[storingFrameIndex].data));
  } catch (const std::exception& e) {
    std::cerr<<"ERROR: Página no númerica"<<std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}