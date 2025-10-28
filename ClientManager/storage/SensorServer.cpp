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
  // enqueue the client message
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
    case MessageType::FILE_NUMBER_REQ: {
      GenNumReq messageContent = getMessageContent<GenNumReq>(clientRequest);
      fileNumberResp resp;
      // resp.totalFiles = 0;  de donde obtengo el total de archivos?
      genMessage reply;
      reply.MID = static_cast<uint8_t>(MessageType::FILE_NUMBER_REP);
      reply.content = resp;
      this->listeningSocket.bSendData(clientSocket, reply);
      this->listeningSocket.closeSocket(clientSocket);
      break;
    }
    case MessageType::SEN_FILE_NAMES_REQ: {
      genSenFileReq messageContent = getMessageContent<genSenFileReq>(clientRequest);
      senFileNamesRes res;
      // Rellenar mensaje
      genMessage reply;
      reply.MID = static_cast<uint8_t>(MessageType::SEN_FILE_NAMES_RES);
      reply.content = res;
      this->listeningSocket.bSendData(clientSocket, reply);
      this->listeningSocket.closeSocket(clientSocket);
      break;
    }
    case MessageType::SEN_FILE_METD_REQ: {
      genSenFileReq messageContent = getMessageContent<genSenFileReq>(clientRequest);
      senFileMetDRes res{};
      // Rellenar mensaje
      genMessage reply;
      reply.MID = static_cast<uint8_t>(MessageType::SEN_FILE_METD_RES);
      reply.content = res;
      this->listeningSocket.bSendData(clientSocket, reply);
      this->listeningSocket.closeSocket(clientSocket);
      break;
    }
    case MessageType::SEN_FILE_BLOCKNUM_REQ: {
      genSenFileReq messageContent = getMessageContent<genSenFileReq>(clientRequest);
      senFileBlockNumRes res{};
      // Rellenar mensaje
      genMessage reply;
      reply.MID = static_cast<uint8_t>(MessageType::SEN_FILE_BLOCKNUM_RES);
      reply.content = res;
      this->listeningSocket.bSendData(clientSocket, reply);
      this->listeningSocket.closeSocket(clientSocket);
      break;
    }
    case MessageType::SEN_FILE_BLOCK_REQ: {
      genSenFileReq messageContent = getMessageContent<genSenFileReq>(clientRequest);
      senFileBlockRes res{};
      // Rellenar mensaje
      genMessage reply;
      reply.MID = static_cast<uint8_t>(MessageType::SEN_FILE_BLOCK_RESP);
      reply.content = res;
      this->listeningSocket.bSendData(clientSocket, reply);
      this->listeningSocket.closeSocket(clientSocket);
      break;
    }
    case MessageType::AUTH_LOGIN_REQ: {
      authLoginReq messageContent = getMessageContent<authLoginReq>(clientRequest);
      authLoginSuccess res{};
      // Rellenar mensaje
      genMessage reply;
      reply.MID = static_cast<uint8_t>(MessageType::AUTH_LOGIN_SUCCESS);
      reply.content = res;
      this->listeningSocket.bSendData(clientSocket, reply);
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
  std::string fileName = this->getSensorFileName(messageContent.fileName);
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