/// @copyright Los Más Fritos - 2025

#ifndef MEMORY_MANAGER_2025B
#define MEMORY_MANAGER_2025B

#include <fstream>
#include "TLB.hpp"
#include "VMSpecifics.hpp"

class VMManager {

 private:
  /// 
  int pageTable[PAGE_TABLE_SIZE];
  /// 
  char physicalMemory[PHYSICAL_MEMORY_SIZE];
  /// 
  const char* backingStoreMem;
  /// 
  int nextFreeFrame;

 public:
  ///
  explicit VMManager(const char* backingStorePtr);
  ///
  ~VMManager() = default;
  ///
  int getFrame(int pageNumber, int& pageFaults);
  ///
  char getValue(int frameNumber, int offset) const;

 private:
  ///
  void handlePageFault(int pageNumber);

};

#endif  // MEMORY_MANAGER_2025B
