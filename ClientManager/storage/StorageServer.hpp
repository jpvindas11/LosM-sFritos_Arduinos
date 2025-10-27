#ifndef STORAGESERVER_H
#define STORAGESERVER_H

#include <iostream>
#include <cstring>
#include <string>


#include "../FileSystem/src/FileSystem.hpp"
#include "../Util/Socket.hpp"


class StorageServer {
 private:
  std::string serverIP;
  int listeningPort;
  std::string masterIP;
  int materPort;
  FileSystem storage;
  Socket listeningSocket;

 public:
  StorageServer();
  ~StorageServer();

  int openConnectionRequestSocket(std::string ip, int port);

  int listenForConnections(std::string ip, int port);

  int acceptConnectionRequest();

  void acceptAllConnections();

  void handleClientConnection(int client);

  void listenForever(std::string ip, int port);
};

#endif //STORAGESERVER_H