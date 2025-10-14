/// @copyright Los Más Fritos - 2025

#include <array>
#include <vector>

#include "VirtualMemoryManager.hpp"

int main() {
  VirtualMemoryManager vmm("src/FileSystem/disk.bin");
  std::vector<int> addressList = {1, 256, 32768, 32769, 128, 65534, 33153};
  vmm.translateAddresses(addressList);
  vmm.printStatics();
  return 0;
}