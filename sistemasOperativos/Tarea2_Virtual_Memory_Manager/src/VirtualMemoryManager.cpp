/// @copyright Los Más Fritos - 2025

#include <iostream>
#include <sstream>
#include <vector>
#include "VirtualMemoryManager.hpp"

VirtualMemoryManager::VirtualMemoryManager(const std::string& backingStorePath)
    : nextFreeFrame(0)
    , pageFaults(0)
    , totalAccesses(0)
    , timeCounter(0) {
  std::memset(pageTable, -1, sizeof(pageTable));
  std::memset(lastUsed, 0, sizeof(lastUsed));
  backingStore.open(backingStorePath, std::ios::binary);
  if (!backingStore.is_open()) {
    std::cerr << "No se pudo abrir el archivo de backing store\n";
    std::exit(1);
  }
}

void VirtualMemoryManager::handlePageFault(int pageNum) {
  int frameToUse;
  if (nextFreeFrame < NUM_FRAMES) {
    frameToUse = nextFreeFrame++;
  } else {
    int lruFrame = 0;
    int minTime = lastUsed[0];
    for (int i = 1; i < NUM_FRAMES; ++i) {
      if (lastUsed[i] < minTime) {
        minTime = lastUsed[i];
        lruFrame = i;
      }
    }
    frameToUse = lruFrame;
    backingStore.seekg(pageNum * PAGE_SIZE, std::ios::beg);
    backingStore.read(&physicalMemory[frameToUse * FRAME_SIZE], PAGE_SIZE);
    for (int i = 0; i < PAGE_TABLE_SIZE; ++i) {
      if (pageTable[i] == frameToUse) {
        pageTable[i] = -1;
        break;
      }
    }
  }
  std::memcpy(&physicalMemory[frameToUse * FRAME_SIZE],
              &backingStoreMem[pageNum * PAGE_SIZE],
              PAGE_SIZE);
  pageTable[pageNum] = frameToUse;
  lastUsed[frameToUse] = timeCounter++;
}

int VirtualMemoryManager::getFrame(int pageNum) {
  int frameNum = pageTable[pageNum];
  if (frameNum == -1) {
    handlePageFault(pageNum);
    this->pageFaults++;
    frameNum = pageTable[pageNum];
  }
  return frameNum;
}

char VirtualMemoryManager::getValue(int frameNum, int offset) const {
  return physicalMemory[frameNum * FRAME_SIZE + offset];
}

void VirtualMemoryManager::translateAddresses(
    const std::vector<int> &addresses) {
  for (int logicalAddress : addresses) {
    totalAccesses++;
    int maskedAddress = logicalAddress & 0xFFFF;
    int pageNumber = (maskedAddress / PAGE_SIZE) % PAGE_TABLE_SIZE;
    int offset = maskedAddress % PAGE_SIZE;
    int frameNum = getFrame(pageNumber);
    int physicalAddress = frameNum * FRAME_SIZE + offset;
    char value = getValue(frameNum, offset);
    std::cout << "Logical address: " << logicalAddress
              << " Physical address: " << physicalAddress
              << " Value: " << static_cast<int>(value) << std::endl;
  }
}

void VirtualMemoryManager::printStatics() const {
  std::cout << "Page-fault rate: "
            << (100.0 * pageFaults / totalAccesses)
            << "%\n";
}
