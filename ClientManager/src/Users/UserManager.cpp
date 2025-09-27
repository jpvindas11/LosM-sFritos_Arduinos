/// @copyright Los Más Fritos - 2025

#include "UserManager.hpp"

UserManager::UserManager(FileSystem& fs)
  : fs(fs) {
  this->disk = "users.bin";
  this->fs.loadFromDisk(this->disk);
}

UserManager::~UserManager(){}

int UserManager::addUser() {}

int UserManager::deleteUser() {}

void UserManager::printUser() {}

int UserManager::changeName(char oldName, char newName) {}

int UserManager::changePassword(char name, char newPassword) {}