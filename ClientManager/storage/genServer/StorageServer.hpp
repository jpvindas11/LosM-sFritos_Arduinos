#ifndef STORAGESERVER_H
#define STORAGESERVER_H

#include <iostream>
#include <cstring>
#include <string>

#include "../../Util/IPConstants.hpp"
#include "../../Util/Messages.hpp"
#include "../../FileSystem/src/FileSystem.hpp"
#include "../../Util/Socket.hpp"
#include "../Util/ServerDiscover.hpp"
#include "../Util/ServerDiscoveryPoint.hpp"

class StorageServer {
 protected:
  std::string serverIP;
  int listeningPort;
  FileSystem storage;
  Socket listeningSocket;

  ServerDiscoveryPoint* discoveryPoint;

 public:
  StorageServer();
  ~StorageServer() = default;

  int openConnectionRequestSocket(std::string ip, int port, std::string name, int disc_port, ServerType type);

  int listenForConnections(std::string ip, int port,
  std::string name, int disc_port, ServerType type);

  int acceptConnectionRequest();

  void acceptAllConnections();

  // Do nothing on base class
  virtual void handleClientConnection(int client) {
    (void)client;
  }

  virtual uint8_t getRaidMode() const {
      return 0;  // Por defecto STANDALONE
  }

  void listenForever(std::string ip, int port, std::string name, int disc_port, ServerType type);
};

#endif //STORAGESERVER_H