/// Los más fritos 2025

#include "MasterServer.hpp"
#include "MasterEntryPoint.hpp"

MasterServer::MasterServer() 
    :
      userPort(PORT_MASTER_USERS),
      arduinoPort(PORT_MASTER_ARDUINO) {
}

MasterServer::~MasterServer() {
  this->userEntryPoint.closeSocket();
  this->arduinoEntryPoint.closeSocket();

  for (auto* listener : listeners) {
    delete listener;
  }
  listeners.clear();
}

void MasterServer::setDestinationIPs(std::string auth, std::string storage, std::string logs) {
  this->authServerIP = auth;
  this->storageServerIP = storage;
  this->logsServerIP = logs;
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
  std::string targetIP;
  int targetPort;

  switch (msgType) {
    case MessageType::AUTH_LOGIN_REQ:
    case MessageType::AUTH_LOGOUT:
    case MessageType::AUTH_USER_CREATE:
    case MessageType::AUTH_USER_DELETE:
    case MessageType::AUTH_USER_MODIFY_PASS:
    case MessageType::AUTH_USER_MODIFY_RANK:
    case MessageType::AUTH_USER_REQUEST:
    targetIP = lookForServer(DISC_AUTH);
    targetPort = PORT_MASTER_AUTH;
    break;

    case MessageType::FILE_NUMBER_REQ:
    case MessageType::SEN_FILE_NAMES_REQ:
    case MessageType::SEN_FILE_METD_REQ:
    case MessageType::SEN_FILE_BLOCKNUM_REQ:
    case MessageType::SEN_FILE_BLOCK_REQ:
    case MessageType::SEN_RECENT_DATA_REQ:
    case MessageType::ADD_SENSOR:
    case MessageType::DELETE_SENSOR:
    case MessageType::MODIFY_SENSOR:
    targetIP = this->storageServerIP;
    targetPort = PORT_MASTER_STORAGE;
    break;

    case MessageType::LOG_USER_REQUEST:
    targetIP = this->logsServerIP;
    targetPort = PORT_MASTER_LOGS;
    break;

    case MessageType::SERVER_STATUS_REQ:
      handleServerStatusRequest(client, clientRequest);
      return;
    default:
    // Error
    targetIP = "ERR";
    targetPort = PORT_MASTER_AUTH;
    break;
  }

  auto* worker = new MasterWorker(client, targetIP, targetPort, clientRequest);
  worker->startThread();
}

void MasterServer::handleArduinoConnection(int client, Socket* socket) {
  std::cout << "[ARDUINO] New connection on client " << client << std::endl;
  
  Socket clientSocket;
  genMessage clientRequest;
  
  if (clientSocket.bReceiveData(client, clientRequest) <= 0) {
    close(client);
    return;
  }

  MessageType msgType = static_cast<MessageType>(clientRequest.MID);
  std::string targetIP;
  int targetPort;

  switch (msgType) {
    case MessageType::SEN_ADD_LOG:
    targetIP = this->storageServerIP;
    targetPort = PORT_MASTER_STORAGE;
    break;
    default:
    // Error
    targetIP = "ERR";
    targetPort = PORT_MASTER_AUTH;
    break;
  }

  auto* worker = new MasterWorker(client, targetIP, targetPort, clientRequest);
  worker->startThread();
}

void MasterServer::handleServerStatusRequest(int client, genMessage& request) {
    std::cout << "========== SERVER STATUS REQUEST ==========" << std::endl;
    std::cout << "Auth IP: '" << this->authServerIP << "'" << std::endl;
    std::cout << "Storage IP: '" << this->storageServerIP << "'" << std::endl;
    std::cout << "Logs IP: '" << this->logsServerIP << "'" << std::endl;
    
    genMessage response;
    response.MID = static_cast<uint8_t>(MessageType::SERVER_STATUS_RES);
    
    serverStatusRes statusRes;
    statusRes.id_token = 0;
    
    // AUTH
    serverStatus authStatus;
    authStatus.serverName = "AUTH";
    authStatus.serverIP = this->authServerIP;
    authStatus.serverPort = PORT_MASTER_AUTH;
    authStatus.isConnected = checkServerConnection(this->authServerIP, PORT_MASTER_AUTH);
    authStatus.lastCheck = static_cast<uint32_t>(time(nullptr));
    statusRes.servers.push_back(authStatus);
    std::cout << "Added AUTH server: " << authStatus.serverIP << ":" 
              << authStatus.serverPort << " - " 
              << (authStatus.isConnected ? "ONLINE" : "OFFLINE") << std::endl;
    
    // STORAGE
    serverStatus storageStatus;
    storageStatus.serverName = "STORAGE";
    storageStatus.serverIP = this->storageServerIP;
    storageStatus.serverPort = PORT_MASTER_STORAGE;
    storageStatus.isConnected = checkServerConnection(this->storageServerIP, PORT_MASTER_STORAGE);
    storageStatus.lastCheck = static_cast<uint32_t>(time(nullptr));
    statusRes.servers.push_back(storageStatus);
    std::cout << "Added STORAGE server: " << storageStatus.serverIP << ":" 
              << storageStatus.serverPort << " - " 
              << (storageStatus.isConnected ? "ONLINE" : "OFFLINE") << std::endl;
    
    // LOGS
    serverStatus logsStatus;
    logsStatus.serverName = "LOGS";
    logsStatus.serverIP = this->logsServerIP;
    logsStatus.serverPort = PORT_MASTER_LOGS;
    logsStatus.isConnected = checkServerConnection(this->logsServerIP, PORT_MASTER_LOGS);
    logsStatus.lastCheck = static_cast<uint32_t>(time(nullptr));
    statusRes.servers.push_back(logsStatus);
    std::cout << "Added LOGS server: " << logsStatus.serverIP << ":" 
              << logsStatus.serverPort << " - " 
              << (logsStatus.isConnected ? "ONLINE" : "OFFLINE") << std::endl;
    
    std::cout << "Total servers in response: " << statusRes.servers.size() << std::endl;
    
    response.content = statusRes;
    
    Socket clientSocket;
    ssize_t sent = clientSocket.bSendData(client, response);
    std::cout << "Response sent: " << sent << " bytes" << std::endl;
    std::cout << "==========================================\n" << std::endl;
    
    close(client);
}

uint8_t MasterServer::checkServerConnection(const std::string& ip, int port) {
    Socket testSocket;
    
    if (!testSocket.create()) {
        return 0;
    }
    
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    setsockopt(testSocket.getSocketFD(), SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(testSocket.getSocketFD(), SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    
    bool connected = testSocket.connectToServer(ip, port);
    
    if (connected) {
        close(testSocket.getSocketFD());
    }
    
    return connected ? 1 : 0;
}

std::string MasterServer::lookForServer(int port) {
    ServerDiscover discover(port);

    std::vector<ServerDiscover::DiscoveredServer> servers = discover.discoverServers(3);

    if (servers.empty()) {
        std::cout << "No se encontraron servidores disponibles" << std::endl;
        return "NOSERVER";
    }

    return servers[0].ip;
}