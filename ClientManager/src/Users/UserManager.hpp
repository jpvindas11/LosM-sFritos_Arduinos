/// @copyright Los Más Fritos - 2025

#ifndef USER_MANAGER_2025
#define USER_MANAGER_2025

#include "FileSystem.hpp"

class UserManager {
 private:
  FileSystem& fs;

 public:
  explicit UserManager(FileSystem& fs)
    : fs(fs) {}
};

#endif  // USER_MANAGER_2025
