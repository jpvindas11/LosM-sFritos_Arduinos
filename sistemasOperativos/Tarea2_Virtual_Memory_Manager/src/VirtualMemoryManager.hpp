#ifndef VIRTUALMEMORYMANGER
#define VIRTUALMEMORYMANGER

#pragma once

#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unordered_map>
#include <iostream>
#include "FileSystem.hpp"

#define FRAME_COUNT 8
#define FRAME_SIZE 1024


typedef struct frame {
  char data [1024];
} frame;

class VirtualMemoryManager {
 private:
  int64_t pageTable[FRAME_COUNT];
  int64_t timeTable[FRAME_COUNT];
  std::vector<frame> framesContent;
  std::unordered_map<std::string,std::string> correspondanceTable;
  FileSystem* disk;
  uint64_t timeCounter;
  uint64_t pageFaults;
  uint64_t tableHits;

 public:
   VirtualMemoryManager();
   ~VirtualMemoryManager();
   void getReferencedPages(std::string& referenceString);


 private:
  void initializeManager();
  int64_t getPhysicalPage(std::string& logicalPage);
  void proccessFileCorrespondenceTable(char* tableContent, 
                                          std::vector<std::string>& directions);
  void fillCorrespondanceTable(std::vector<std::string>& pages);
  int64_t getFreePage();
  bool checkPageInTable(int64_t page);
  int allocatePage(std::string& requestedPage, int64_t storingFrameIndex);
};

#endif // VIRTUALMEMORYMANGER