/// Los más fritos 2025

#include "MasterServer.hpp"
#include "MasterEntryPoint.hpp"

MasterServer::MasterServer() 
    :
      userPort(PORT_MASTER_USERS),
      arduinoPort(PORT_MASTER_ARDUINO),
      discoveryPoint(nullptr) {
}

MasterServer::~MasterServer() {
  this->userEntryPoint.closeSocket();
  this->arduinoEntryPoint.closeSocket();

  if (discoveryPoint) {
    discoveryPoint->stopDiscovery();
    discoveryPoint->waitToFinish();
    delete discoveryPoint;
  }

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

  discoveryPoint = new ServerDiscoveryPoint(
      "MASTER_SERVER_FRI",
      ip,
      DISC_MASTER,
      ServerType::SV_MASTER
  );
  
  if (discoveryPoint->startThread() != EXIT_SUCCESS) {
    std::cerr << "! Could not start discovery thread" << std::endl;
    delete discoveryPoint;
    discoveryPoint = nullptr;
    return EXIT_FAILURE;
  }


  std::cout << "All listeners started successfully" << std::endl;
  return EXIT_SUCCESS;
}

void MasterServer::waitForAllListeners() {
  for (auto* listener : listeners) {
    listener->waitToFinish();
  }

  if (discoveryPoint) {
    discoveryPoint->waitToFinish();
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
  
  genMessage clientRequest;
  if (socket->bReceiveData(client, clientRequest) <= 0) {
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
    case MessageType::AUTH_USER_REQUEST: {
      ServerDiscover discoverer(DISC_AUTH);
      targetIP = discoverer.lookForServer();
      targetPort = PORT_MASTER_AUTH;
      break;
    }
    
    case MessageType::FILE_NUMBER_REQ:
    case MessageType::SEN_FILE_NAMES_REQ:
    case MessageType::SEN_FILE_METD_REQ:
    case MessageType::SEN_FILE_BLOCKNUM_REQ:
    case MessageType::SEN_FILE_BLOCK_REQ:
    case MessageType::SEN_RECENT_DATA_REQ:
    case MessageType::ADD_SENSOR:
    case MessageType::DELETE_SENSOR:
    case MessageType::MODIFY_SENSOR: 
    {
      ServerDiscover discoverer(DISC_STORAGE);
      targetIP = discoverer.lookForServer();
      targetPort = PORT_MASTER_STORAGE;
      break;
    }
    
    case MessageType::LOG_USER_REQUEST: {
      ServerDiscover discoverer(DISC_USER_LOGS);
      targetIP = discoverer.lookForServer();
      targetPort = PORT_MASTER_LOGS;
      break;
    }
    
    case MessageType::SERVER_STATUS_REQ:
      handleServerStatusRequest(client, clientRequest);
      return;
    
    default:
      std::cerr << "[ERROR] Unknown message type: " 
        << static_cast<int>(clientRequest.MID) << std::endl;
      targetIP = "ERR";
      targetPort = PORT_MASTER_AUTH;
      break;
  }
  
  // Verificar si se encontró un servidor válido
  if (targetIP == "NOSERVER" || targetIP == "ERR") {
    std::cerr << "[ERROR] No suitable server found for message type: " 
      << static_cast<int>(msgType) << std::endl;
    
    // Enviar mensaje de error al cliente
    genMessage errorResponse;
    errorResponse.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
    errorCommonMsg errMsg;
    errMsg.message = "No hay servidores disponibles";
    errorResponse.content = errMsg;
    socket->bSendData(client, errorResponse);
    
    close(client);
    return;
  }
  
  std::cout << "[ROUTING] Forwarding to " << targetIP << ":" << targetPort << std::endl;
  
  auto* worker = new MasterWorker(client, targetIP, targetPort, clientRequest);
  worker->startThread();
}

void MasterServer::handleArduinoConnection(int client, Socket* socket) {
  std::cout << "[ARDUINO] New connection on client " << client << std::endl;
  
  Socket clientSocket;
  genMessage clientRequest;

  ServerDiscover discoverer(DISC_STORAGE);
  
  if (clientSocket.bReceiveData(client, clientRequest) <= 0) {
    close(client);
    return;
  }

  MessageType msgType = static_cast<MessageType>(clientRequest.MID);
  std::string targetIP;
  int targetPort;

  switch (msgType) {
    case MessageType::SEN_ADD_LOG:
    targetIP = discoverer.lookForServer();
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
    
    genMessage response;
    response.MID = static_cast<uint8_t>(MessageType::SERVER_STATUS_RES);
    
    serverStatusRes statusRes;
    statusRes.id_token = 0;
    
    std::vector<std::pair<int, std::string>> discoveryPorts = {
        {DISC_AUTH, "AUTH"},
        {DISC_STORAGE, "STORAGE"},
        {DISC_USER_LOGS, "USER_LOGS"},
        {DISC_SENSOR_PROXY, "PROXY"}
    };
    
    time_t currentTime = time(nullptr);
    int totalServersFound = 0;

    for (const auto& [port, serverTypeName] : discoveryPorts) {
        std::cout << "\n🔍 Buscando servidores " << serverTypeName 
                  << " en puerto " << port << "..." << std::endl;
        
        ServerDiscover discoverer(port);

        std::vector<ServerDiscover::DiscoveredServer> discovered = 
            discoverer.discoverServers(2);
        
        if (discovered.empty()) {
            std::cout << "   ⚠️  No se encontraron servidores " << serverTypeName << std::endl;

            serverStatus offlineStatus;
            offlineStatus.serverName = serverTypeName;
            offlineStatus.serverIP = "N/A";
            offlineStatus.serverPort = getServerPortByType(serverTypeName);
            offlineStatus.isConnected = 0;
            offlineStatus.lastCheck = static_cast<uint32_t>(currentTime);
            
            statusRes.servers.push_back(offlineStatus);
            continue;
        }
    
        for (const auto& server : discovered) {
            serverStatus status;
            status.serverName = server.name;
            status.serverIP = server.ip;
            status.serverPort = getServerPortByDiscovery(port);
            status.isConnected = 1;
            status.lastCheck = static_cast<uint32_t>(currentTime);
            
            statusRes.servers.push_back(status);
            totalServersFound++;
            
            std::cout << "Encontrado: " << server.name 
                      << " (" << server.ip << ":" << status.serverPort << ")" 
                      << std::endl;
        }
    }
    
    response.content = statusRes;
    
    Socket clientSocket;
    ssize_t sent = clientSocket.bSendData(client, response);
    
    if (sent > 0) {
        std::cout << "Respuesta enviada: " << sent << " bytes" << std::endl;
    } else {
        std::cerr << "Error al enviar respuesta" << std::endl;
    }
    
    close(client);
}

uint16_t MasterServer::getServerPortByDiscovery(int discoveryPort) {
    switch (discoveryPort) {
        case DISC_AUTH:
            return PORT_MASTER_AUTH;
        case DISC_STORAGE:
            return PORT_MASTER_STORAGE;
        case DISC_USER_LOGS:
            return PORT_MASTER_LOGS;
        case DISC_SENSOR_PROXY:
            return PORT_PROXY_LISTENER;
        default:
            std::cerr << "⚠️  Puerto de discovery desconocido: " << discoveryPort << std::endl;
            return 0;
    }
}

uint16_t MasterServer::getServerPortByType(const std::string& serverType) {
    if (serverType == "AUTH") return PORT_MASTER_AUTH;
    if (serverType == "STORAGE") return PORT_MASTER_STORAGE;
    if (serverType == "USER_LOGS") return PORT_MASTER_LOGS;
    if (serverType == "PROXY") return PORT_PROXY_LISTENER;
    return 0;
}

uint8_t MasterServer::checkServerConnection(const std::string& ip, int port) {
    if (ip == "N/A" || ip.empty()) {
        return 0;
    }
    
    Socket testSocket;
    
    if (!testSocket.create()) {
        return 0;
    }
    
    // Timeout de 2 segundos para la prueba de conexión
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