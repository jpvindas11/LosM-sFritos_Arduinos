#include "SensorServer.hpp"

SensorServer::SensorServer() : running(false) {
}

SensorServer& SensorServer::getInstance() {
  static SensorServer server;
  return server;
}

int SensorServer::startServer(std::string serverIP, int listeningPort, 
                                          std::string masterIP, int materPort) {
  this->serverIP = serverIP;
  this->listeningPort = listeningPort;
  this->masterIP = masterIP;
  this->materPort = materPort;
  if (!this->storage.mount("sensorStorage.bin")) {
    return EXIT_FAILURE;
  }
  this->listenForever(this->serverIP, this->listeningPort);
  return EXIT_SUCCESS;
}

void SensorServer::stopServer() {  
  running.store(false);
  this->storage.unmount();
}

void SensorServer::closeListeningSocket() {
  this->listeningSocket.closeSocket();
}

void SensorServer::run(std::string serverIP, int listeningPort, 
                                          std::string masterIP, int materPort){
  try{
    if (this->startServer(serverIP, listeningPort, masterIP, materPort) ==
                                                                 EXIT_SUCCESS) {
      running.store(true);
      std::cout << "Storage server started on " << serverIP << ":" 
                << listeningPort << " (multi-threaded mode)\n";
      this->acceptAllConnections();
    }
  } catch(const std::runtime_error& error) {
    std::cerr<<error.what()<<std::endl;
  }
  this->stopServer();
}

void SensorServer::handleClientConnection(int clientSocket) {
  // Crear un thread separado para manejar cada cliente
  std::thread clientThread(&SensorServer::processClientInThread, this, clientSocket);
  clientThread.detach(); // Permitir que el thread se ejecute independientemente
}

void SensorServer::processClientInThread(int clientSocket) {
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

void SensorServer::serveClient(int clientSocket, genMessage& clientRequest) {
  switch (static_cast<MessageType>(clientRequest.MID)) {
    case MessageType::SEN_ADD_LOG: {
      senAddLog messageContent = getMessageContent<senAddLog>(clientRequest);
      this->addToSensorLog(messageContent);
      break;
    }
    default: {
      std::cerr<<"ERROR: MID non recognized"<<std::endl;
      break;
    }
  }
}

void SensorServer::addToSensorLog(senAddLog& messageContent) {
  std::lock_guard<std::mutex> lock(storageMutex); // Thread-safe file access
  
  std::string fileName = this->getSensorFileName(messageContent.fileName);
  std::cout << "Adding to sensor log: " << fileName << std::endl;
  
  // Obtener tamaño actual del archivo
  uint32_t sizeBeforeWrite = 0;
  if (this->storage.fileExists(fileName)) {
    sizeBeforeWrite = this->storage.getFileSize(fileName);
  } else {
    this->storage.createFile(fileName);
  }
  
  // Agregar datos del sensor
  uint32_t dataSize = messageContent.data.size();
  this->storage.appendFile(fileName, messageContent.data.data(), dataSize);
  
  // Add newline for better formatting
  std::string newline = "\n";
  this->storage.appendFile(fileName, newline.data(), newline.size());
  
  // Calcular nuevo tamaño y mostrar info
  uint32_t newSize = sizeBeforeWrite + dataSize + newline.size();
  std::cout << "Agregados " << (dataSize + newline.size()) << " bytes a " << fileName 
            << " (tamaño anterior: " << sizeBeforeWrite 
            << ", nuevo: " << newSize << ")" << std::endl;
  
  // Leer solo las últimas líneas del archivo para mostrar
  const uint32_t READ_SIZE = 1024;
  char buffer[READ_SIZE];
  memset(buffer, 0, READ_SIZE); // Inicializar con ceros
  
  uint32_t totalFileSize = this->storage.getFileSize(fileName);
  uint32_t readSize = std::min(totalFileSize, READ_SIZE - 1); // Dejar espacio para null terminator
  
  this->storage.readFile(fileName, buffer, readSize);
  buffer[readSize] = '\0'; // Asegurar terminación nula
  
  std::cout << "Received for file -> " << fileName << std::endl;
  std::cout << buffer << std::endl;
}

std::string SensorServer:: getSensorFileName(sensorFileName& name) {
  char id [65535];
  char year [10000];
  char month [12];
  char day [32];
  sprintf(id, "%u", name.id);
  sprintf(year, "%u", name.year);
  sprintf(month, "%u", name.month);
  sprintf(day, "%u", name.day);
  std::string idS (id);
  std::string yearS (year);
  std::string monthS (month);
  std::string dayS (day);
  return name.sensorType + "" + idS + "" + yearS + monthS + dayS;
}