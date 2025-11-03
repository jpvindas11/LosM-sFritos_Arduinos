#ifndef STORAGESERVER_H
#define STORAGESERVER_H

#include <iostream>
#include <cstring>
#include <string>


#include "../../FileSystem/src/FileSystem.hpp"
#include "../../Util/Socket.hpp"


class StorageServer {
 protected:
  std::string serverIP;
  int listeningPort;
  FileSystem storage;
  Socket listeningSocket;

 public:
  StorageServer();
  ~StorageServer() = default;

  int openConnectionRequestSocket(std::string ip, int port);

  int listenForConnections(std::string ip, int port);

  int acceptConnectionRequest();

  void acceptAllConnections();

  // Do nothing on base class
  virtual void handleClientConnection(int client) {
    (void)client;
  }

  void listenForever(std::string ip, int port);
};

#endif //STORAGESERVER_H