/// @copyright Los Más Fritos - 2025

#include <iostream>
#include <vector>
#include "VirtualMemoryManager.hpp"

#define PAGE_SIZE = 256; // 8 bits para offset
#define PAGE_TABLE_SIZE = 256; // 8 bits para page number

int main() {
  /*
  std::vector<int> address_list = {1, 256, 32768, 32769, 128, 65534, 33153};

  std::cout << "Address"  << '\t'
            << "Page Num" << '\t'
            << "Offset"   << '\t'
            << std::endl;

  for (int address : address_list) {
    int masked_address = address & 0xFFFF; // 16 bits
    int page_number = (masked_address >> 8) & 0xFF; // bits 8-15
    int offset = masked_address & 0xFF; // bits 0-7

    std::cout << address << '\t'
              << page_number     << '\t'
              << offset          << '\t'
              << std::endl;
  }
  */
  VirtualMemoryManager MMV = VirtualMemoryManager();
  std::string references = "31,30,29,28,31,30,27,26,25,24,5,6,7";
  MMV.getReferencedPages(references);
  MMV.printStatics();
   return 0;
}
