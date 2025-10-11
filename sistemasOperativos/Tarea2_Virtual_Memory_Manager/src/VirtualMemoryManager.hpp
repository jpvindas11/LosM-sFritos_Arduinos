#ifndef VIRTUALMEMORYMANGER
#define VIRTUALMEMORYMANGER

#pragma once

#include <vector>
#include <string>
#include <cstring>
#include <unordered_map>
#include <iostream>
#include "FileSystem.hpp"

#define FRAME_COUNT 8


class VirtualMemoryManager {
 private:
  int64_t pageTable[FRAME_COUNT];
  int64_t timeTable[FRAME_COUNT];
  std::unordered_map<std::string,std::string> correspondanceTable;
  FileSystem* disk;
  uint64_t time_counter;

 public:
   VirtualMemoryManager();
   ~VirtualMemoryManager();
   void getReferencedPages(std::string& referenceString);


 private:
  void initializeManager();
  int64_t getPhysicalPage(uint32_t logicalPage);
  void proccessFileCorrespondenceTable(char* tableContent, 
                                          std::vector<std::string>& directions);
  void fillCorrespondanceTable(std::vector<std::string>& pages);

};

#endif // VIRTUALMEMORYMANGER