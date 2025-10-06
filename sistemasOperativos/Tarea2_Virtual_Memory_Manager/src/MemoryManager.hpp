/// @copyright Los Más Fritos - 2025

#include <fstream>
#include "TLB.hpp"

#define OFFSET_MASK 0x00FF
#define PAGE_TABLE_SIZE 256
#define FRAME_SIZE 256
#define MEMORY_SIZE 65536

class MemoryManager {
public:
  MemoryManager();
  ~MemoryManager();
};