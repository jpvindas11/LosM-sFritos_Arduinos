/// Los más fritos 2025

#include "MasterServer.hpp"
#include "MasterEntryPoint.hpp"

MasterServer::MasterServer() 
    :
      userPort(PORT_MASTER_USERS),
      arduinoPort(PORT_MASTER_ARDUINO) {
}

MasterServer::~MasterServer() {
  for (auto* listener : listeners) {
    delete listener;
  }
  listeners.clear();
}

int MasterServer::startAllListeners(const std::string& ip) {
  serverIP = ip;

  auto* userListener = new MasterEntryPoint(this, &userEntryPoint, ip, userPort, &MasterServer::handleUserConnection);
  listeners.push_back(userListener);

  auto* arduinoListener = new MasterEntryPoint(this, &arduinoEntryPoint, ip, arduinoPort, &MasterServer::handleArduinoConnection);
  listeners.push_back(arduinoListener);

  for (auto* listener : listeners) {
    if (listener->startThread() != EXIT_SUCCESS) {
      std::cerr << "! Could not start listener thread" << std::endl;
      return EXIT_FAILURE;
    }
  }

  std::cout << "All listeners started successfully" << std::endl;
  return EXIT_SUCCESS;
}

void MasterServer::waitForAllListeners() {
  for (auto* listener : listeners) {
    listener->waitToFinish();
  }
}

int MasterServer::openConnectionEntries(std::string ip, int port, Socket* socket) {
  if (!socket->create()) {
    std::cerr << "! Could not set the listening socket on port " << port << std::endl;
    return EXIT_FAILURE;
  }
  if (!socket->setReuseAddr()) {
    std::cerr << "! Could not reuse the listening socket on port " << port << std::endl;
    return EXIT_FAILURE;
  }
  if (!socket->bindSocket(ip, port)) {
    std::cerr << "! Could not connect Socket to port " << port << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int MasterServer::listenForConnections(std::string ip, int port, Socket* socket) {
  if (this->openConnectionEntries(ip, port, socket) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }
  if (!socket->listenSocket()) {
    std::cerr << "! Could not listen for connections on port "  << port << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Listening on port " << port << std::endl;
  return EXIT_SUCCESS;
}

int MasterServer::acceptConnectionRequest(Socket* socket, void (MasterServer::*handler)(int, Socket*)) {
  int clientSocket = socket->acceptConnection();

  if (clientSocket == -1) {
    throw std::runtime_error("could not accept client connection");
  }

  (this->*handler)(clientSocket, socket);

  return 0;
}

void MasterServer::acceptAllConnections(Socket* socket, void (MasterServer::*handler)(int, Socket*)) {
  while (socket->getSocketFD() >= 0) {
    try {
      this->acceptConnectionRequest(socket, handler);
    } catch (const std::exception& e) {
      std::cerr << "Could not accept client connection: " << e.what() << std::endl;
    }
  }
}

void MasterServer::listenForever(std::string ip, int port, Socket* socket,
  void (MasterServer::*handler)(int, Socket*)) {
  this->listenForConnections(ip, port, socket);
  this->acceptAllConnections(socket, handler);
}

// Logica por handler

void MasterServer::handleUserConnection(int client, Socket* socket) {
  std::cout << "[USER] New connection on client " << client << std::endl;

  Socket clientSocket;
  genMessage clientRequest;
  
  if (clientSocket.bReceiveData(client, clientRequest) <= 0) {
    close(client);
    return;
  }

  MessageType msgType = static_cast<MessageType>(clientRequest.MID);
  std::string targetIP = "127.0.0.1";
  int targetPort;

  switch (msgType) {
    case MessageType::AUTH_LOGIN_REQ:
    case MessageType::AUTH_LOGOUT:
    case MessageType::AUTH_USER_CREATE:
    case MessageType::AUTH_USER_DELETE:
    case MessageType::AUTH_USER_MODIFY_PASS:
    case MessageType::AUTH_USER_MODIFY_RANK:
    case MessageType::AUTH_USER_REQUEST:
    targetPort = PORT_MASTER_AUTH;
    break;
    default:
    // Error
    targetPort = PORT_MASTER_AUTH;
    break;
  }

  auto* worker = new MasterWorker(client, targetIP, targetPort, clientRequest);
  worker->startThread();
}

void MasterServer::handleAuthConnection(int client, Socket* socket) {
  std::cout << "[AUTH] New connection on client " << client << std::endl;
  

  socket->closeSocket(client);
}

void MasterServer::handleArduinoConnection(int client, Socket* socket) {
  std::cout << "[ARDUINO] New connection on client " << client << std::endl;
  

  socket->closeSocket(client);
}

void MasterServer::handleStorageConnection(int client, Socket* socket) {
  std::cout << "[STORAGE] New connection on client " << client << std::endl;
  

  socket->closeSocket(client);
}