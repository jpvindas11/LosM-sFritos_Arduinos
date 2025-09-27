#include "AuthenticationServer.hpp"
#include <fstream>
#include <sstream>
#include <sodium.h>

AuthenticationServer::AuthenticationServer(FileSystem& fs)
    : fileSystem(fs), serverRunning(false) {
  if (sodium_init() < 0) {
    throw std::runtime_error("Failed to initialize libsodium");
  }
  char data[10000]; // 10KB buffer
  int bytesRead = fileSystem.read(userDataFile, 0, 10000, data);
  std::istringstream ss(std::string(data, bytesRead));
  std::string line;
  while (std::getline(ss, line)) {
    std::istringstream lineStream(line);
    std::string username, salt, hash;
    if (std::getline(lineStream, username, ',') &&
        std::getline(lineStream, salt, ',') &&
        std::getline(lineStream, hash)) {
      users.push_back({username, salt, hash});
    }
  }
}

AuthenticationServer::~AuthenticationServer() {
  this->stopServer();
  this->waitToFinish();
}

