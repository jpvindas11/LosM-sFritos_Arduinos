/// @copyright Los Más Fritos - 2025

#ifndef VIRTUALMEMORYMANGER
#define VIRTUALMEMORYMANGER

#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "common.hpp"
#include "VMSpecifics.hpp"

class VirtualMemoryManager {
  DISABLE_COPY(VirtualMemoryManager);

 private:
  ///
  int pageTable[PAGE_TABLE_SIZE];
  ///
  char physicalMemory[PHYSICAL_MEMORY_SIZE];
  ///
  const char* backingStoreMem;
  ///
  int nextFreeFrame;
  ///
  int pageFaults;
  ///
  int totalAccesses;
 public:
  ///
  explicit VirtualMemoryManager(const char* backingStorePtr);
  ///
  ~VirtualMemoryManager() = default;
  ///
  int getFrame(int pageNumber);
  ///
  char getValue(int frameNum, int offset) const;
  ///
  void translateAddresses(const std::vector<int>& addresses);
  ///
  void printStatics() const;
 private:
  ///
  void handlePageFault(int pageNumber);
};

#endif  // VIRTUALMEMORYMANGER
