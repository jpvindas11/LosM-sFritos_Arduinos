#include "SensorServer.hpp"

SensorServer::SensorServer(){
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
      this->acceptAllConnections();
    }
  } catch(const std::runtime_error& error) {
    std::cerr<<error.what()<<std::endl;
  }
  this->stopServer();
}

void SensorServer::handleClientConnection(int clientSocket) {
  genMessage clientRequest;
  this->listeningSocket.bReceiveData(clientSocket, clientRequest);
  // TODO: in the future use threads to handle the client´s message, so use a queue to
  // enquee the client message
  this->serveClient(clientSocket, clientRequest);
}

void SensorServer::serveClient(int clientSocket, genMessage& clientRequest) {
  switch (static_cast<MessageType>(clientRequest.MID)) {
    case MessageType::SEN_ADD_LOG: {
      senAddLog messageContent = getMessageContent<senAddLog>(clientRequest);
      this->addToSensorLog(messageContent);
      this->listeningSocket.closeSocket(clientSocket);
      break;
    }
    default: {
      std::cerr<<"ERROR: MID non recognized"<<std::endl;
      break;
    }
  }
}

void SensorServer::addToSensorLog(senAddLog& messageContent) {
  if (!this->storage.fileExists(fileName)) {
    this->storage.createFile(fileName);
  }
  this->storage.appendFile(fileName, messageContent.data.data(), 
                                                    messageContent.data.size());
  char buffer [256];
  uint32_t size = 256;
  this->storage.readFile(fileName, buffer, size);
  std::cout<<"Received for file -> "<<fileName<<"\n"<<buffer<<std::endl;
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
  return name.sensorType + "_" + idS + "_" + yearS + monthS + dayS;
}