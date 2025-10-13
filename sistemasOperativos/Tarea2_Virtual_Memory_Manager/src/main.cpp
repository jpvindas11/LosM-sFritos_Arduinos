/// @copyright Los Más Fritos - 2025

#include <array>
#include <vector>

#include "VirtualMemoryManager.hpp"

int main() {
  std::array<char, PHYSICAL_MEMORY_SIZE> backingStore;
  // Inicializa el backing store con un patrón
  for (size_t i = 0; i < backingStore.size(); ++i) {
    backingStore[i] = static_cast<char>(i % 128);
  }

  VirtualMemoryManager vmm(backingStore.data());
  std::vector<int> addressList = {1, 256, 32768, 32769, 128, 65534, 33153};
  vmm.translateAddresses(addressList);
  vmm.printStatics();
  return 0;
}
