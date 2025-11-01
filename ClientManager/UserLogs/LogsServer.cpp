#include "LogsServer.hpp"

LogsServer::LogsServer() : running(false) {
}

LogsServer& LogsServer::getInstance() {
  static LogsServer server;
  return server;
}

int LogsServer::startServer(std::string serverIP, int listeningPort, 
                                          std::string masterIP, int materPort) {
  this->serverIP = serverIP;
  this->listeningPort = listeningPort;
  this->masterIP = masterIP;
  this->materPort = materPort;
  if (!this->storage.mount("userLogsStorage.bin")) {
    return EXIT_FAILURE;
  }
  this->listenForever(this->serverIP, this->listeningPort);
  return EXIT_SUCCESS;
}

void LogsServer::stopServer() {  
  running.store(false);
  this->storage.unmount();
}

void LogsServer::closeListeningSocket() {
  this->listeningSocket.closeSocket();
}

void LogsServer::run(std::string serverIP, int listeningPort, 
                                          std::string masterIP, int materPort){
  try{
    if (this->startServer(serverIP, listeningPort, masterIP, materPort) ==
                                                                 EXIT_SUCCESS) {
      running.store(true);
      std::cout << "Logs server started on " << serverIP << ":" 
                << listeningPort << " (multi-threaded mode)\n";
      this->acceptAllConnections();
    }
  } catch(const std::runtime_error& error) {
    std::cerr<<error.what()<<std::endl;
  }
  this->stopServer();
}

void LogsServer::handleClientConnection(int clientSocket) {
  // Crear un thread separado para manejar cada cliente
  std::thread clientThread(&LogsServer::processClientInThread, this, clientSocket);
  clientThread.detach(); // Permitir que el thread se ejecute independientemente
}

void LogsServer::processClientInThread(int clientSocket) {
  std::cout << "Processing client in thread (socket: " << clientSocket << ")\n";
  
  try {
    genMessage clientRequest;
    ssize_t bytesReceived = this->listeningSocket.bReceiveData(clientSocket, clientRequest);
    
    if (bytesReceived > 0) {
      std::cout << "Received message from client (MID: " 
                << static_cast<int>(clientRequest.MID) << ")\n";
      this->serveClient(clientSocket, clientRequest);
      std::cout << "Client served successfully (socket: " << clientSocket << ")\n";
    } else {
      std::cerr << "WARNING: Could not receive data from client (socket: " 
                << clientSocket << ")\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "Error handling client (socket: " << clientSocket << "): " 
              << e.what() << std::endl;
  }
  
  // Cerrar el socket del cliente
  std::cout << "Closing client socket: " << clientSocket << std::endl;
  this->listeningSocket.closeSocket(clientSocket);
}

void LogsServer:: serveClient(int clientSocket, genMessage& clientRequest) {
  
}