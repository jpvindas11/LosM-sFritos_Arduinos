/// @copyright Los Más Fritos - 2025

#include <vector>

#include "VirtualMemoryManager.hpp"

int main() {
    VirtualMemoryManager vmm;
    std::vector<int> address_list = {1, 256, 32768, 32769, 128, 65534, 33153};
    vmm.translateAddresses(address_list);
    vmm.printStatics();
    return 0;
}
