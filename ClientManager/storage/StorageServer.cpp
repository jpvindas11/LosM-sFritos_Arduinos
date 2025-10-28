#include <stdexcept>

#include "StorageServer.hpp"

StorageServer::StorageServer(){
}

StorageServer::~StorageServer(){
}

int StorageServer::openConnectionRequestSocket(std::string ip, int port){
  if (!this->listeningSocket.create()) {
    std::cerr<<"ERROR: Could not set the listening socket"<<std::endl;
    return EXIT_FAILURE;
  }
  if (!this->listeningSocket.setReuseAddr()) {
        std::cerr<<"ERROR: Could not reuse the listening socket"<<std::endl;
        return EXIT_FAILURE;
  }
  if (!this->listeningSocket.bindSocket(ip, port)) {
      std::cerr<<"ERROR: Could not connect Socket to port"<<std::endl;
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int StorageServer::listenForConnections(std::string ip, int port) {
  if (this->openConnectionRequestSocket(ip, port) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }
  if (!this->listeningSocket.listenSocket()) {
      std::cerr<<"ERROR: Could not listen for connections"<<std::endl;
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int StorageServer::acceptConnectionRequest() {

  int clientSocket = this->listeningSocket.acceptConnection();

  if (clientSocket == -1) {
    throw std::runtime_error("could not accept client connection");
  }

  this->handleClientConnection(clientSocket);

  return clientSocket;
}

void StorageServer::acceptAllConnections(){
  while(this->listeningSocket.getSocketFD() >= 0) {
    this->acceptConnectionRequest();
  }
}

void StorageServer::listenForever(std::string ip, int port) {
  this->listenForConnections(ip, port);
  this->acceptAllConnections();
}