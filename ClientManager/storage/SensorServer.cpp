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
      // Cliente solicita cantidad de archivos
      GenNumReq messageContent = getMessageContent<GenNumReq>(clientRequest);
      fileNumberResp resp;
      break;
    }

    case MessageType::SEN_FILE_NAMES_REQ: {
      // Cliente pide nombres de archivos (paginado)
      genSenFileReq messageContent = getMessageContent<genSenFileReq>(clientRequest);
      senFileNamesRes res;
      // TODO rellenar respuesta
      genMessage reply;
      reply.MID = static_cast<uint8_t>(MessageType::SEN_FILE_NAMES_RES);
      reply.content = res;
      this->listeningSocket.bSendData(clientSocket, reply);
      this->listeningSocket.closeSocket(clientSocket);
      break;
    }

    case MessageType::SEN_FILE_METD_REQ: {
      genSenFileReq messageContent = getMessageContent<genSenFileReq>(clientRequest);
      this->sendSensorFileMetadata(clientSocket, messageContent);
      break;
    }

    case MessageType::SEN_FILE_BLOCKNUM_REQ: {
      genSenFileReq req = getMessageContent<genSenFileReq>(clientRequest);
      break;
    }

    case MessageType::SEN_FILE_BLOCK_REQ: {
      genSenFileReq req = getMessageContent<genSenFileReq>(clientRequest);
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

std::string SensorServer::getSensorFileName(sensorFileName& name) {
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
  return name.sensorType + "_" + idS + "_" + yearS + "/" + 
         monthS + "/" + dayS + ".log";
}

void SensorServer::sendSensorFileMetadata(int clientSocket, genSenFileReq messageContent) {
  // Crea la respuesta
  genMessage reply;
  // Obtiene el nombre del sensor
  std::string filename = this->getSensorFileName(messageContent.fileName);
  iNode inode;
  // Trata de buscar los metadatos
  if (this->storage.getFileInfo(filename, &inode)) {
    senFileMetDRes res;
    res.fileName = messageContent.fileName;
    res.size = this->storage.getFileSize(filename);
    res.permissions = inode.permissions;
    res.userId = inode.userId;
    res.groupId = inode.groupId;
    res.creationTime = inode.creationTime;
    res.lastModifiedTime = inode.lastModifiedTime;
    res.lastAccessTime = inode.lastAccessTime;
    // Llena la respuesta si hay éxito
    reply.MID = static_cast<uint8_t>(MessageType::SEN_FILE_METD_RES);
    reply.content = res;
  } else {
    // Crea un mensaje de error si no
    errorCommonMsg err;
    err.message = "No se pudo obtener los metadatos del archivo " + filename;
    reply.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
    reply.content = err;
  }
  this->listeningSocket.bSendData(clientSocket, reply);
  this->listeningSocket.closeSocket(clientSocket);
}
