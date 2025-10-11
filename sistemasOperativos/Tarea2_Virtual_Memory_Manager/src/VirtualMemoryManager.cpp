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
    std::cerr<<"ERROR: La página "<< requestedPage<<" no existe"<<std::endl;
    return EXIT_FAILURE;
  }
  try {
    int64_t numericPage = std::stoll(requestedPage);
    this->pageTable[storingFrameIndex] = numericPage;
    this->timeTable[storingFrameIndex] = this->timeCounter;
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

void VirtualMemoryManager:: getReferencedPages(std::string& referenceString){
  
  std::stringstream ss(referenceString);
  std::string logicalPage;
  while (std::getline(ss, logicalPage, ',')) {
    if (!logicalPage.empty()) {
      if (!this->checkPageInTable(std::stoll(logicalPage))) {
        this->pageFaults++;
        this->LRUBringPage(logicalPage);
      } else {
          this->tableHits++;
          std::cout<<"La página "<<logicalPage<<" ya fue añadida a la tabla"
                   << std::endl;
      }
      this->timeCounter++;
    }
  } 
}

int VirtualMemoryManager::LRUBringPage(std::string& requestedPage) {
  int64_t emptyPage = this->getFreePage();
  if (emptyPage == -1) {
    emptyPage = this->LRUSelectVictimFrame();
  }
  this->allocatePage(requestedPage, emptyPage);
  std::cout<<"Página: "<<requestedPage<<" cargada en "<<emptyPage<<std::endl;
  //std::cout<<this->framesContent[emptyPage].data<<std::endl;
  return EXIT_SUCCESS;
}

int64_t VirtualMemoryManager:: LRUSelectVictimFrame() {
  int64_t frameIndex = this->getOldestFrame();
  this->cleanFrame(frameIndex);
  return frameIndex;
}

int64_t VirtualMemoryManager::getOldestFrame(){
  int64_t oldestTime = this->timeTable[0];
  int64_t frameIndex = 0;
  for (int64_t index = 0; index < FRAME_COUNT; index++){
    if (this->timeTable[index] < oldestTime) {
      oldestTime = this->timeTable[index];
      frameIndex = index; 
    }
  }
  return frameIndex;
}

void VirtualMemoryManager::cleanFrame(int64_t frameNumber) {
  this->pageTable[frameNumber] = -1;
  this->timeTable[frameNumber] = -1;
  memset(this->framesContent[frameNumber].data,'\0', 
                                 sizeof(this->framesContent[frameNumber].data));
}

void VirtualMemoryManager::printStatics() {
  double pageFaultPercentage = 0;
  double tableHitsPercentage = 0;
  if (this->timeCounter != 0) {
    pageFaultPercentage = 
                       ((double)this->pageFaults/(double)this->timeCounter)*100;
    tableHitsPercentage = 
                       ((double)this->tableHits/(double)this->timeCounter)*100;
  }
  std::cout<<"Cantidad de page faults: "<<this->pageFaults
           <<". "<<pageFaultPercentage<<"\% de las solicitudes realizadas"
           <<std::endl;
  std::cout<<"Cantidad de table hits: "<<this->tableHits
           <<". "<<tableHitsPercentage<<"\% de las solicitudes realizadas"
           <<std::endl;
}