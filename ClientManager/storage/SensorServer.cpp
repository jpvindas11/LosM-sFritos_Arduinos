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
      this->sendFileNumber(clientSocket, messageContent);
      break;
    }

    case MessageType::SEN_FILE_NAMES_REQ: {
      GenNumReq messageContent = getMessageContent<GenNumReq>(clientRequest);
      this->sendFileNames(clientSocket, messageContent);
      break;
    }

    case MessageType::SEN_FILE_METD_REQ: {
      genSenFileReq messageContent = getMessageContent<genSenFileReq>(clientRequest);
      this->sendSensorFileMetadata(clientSocket, messageContent);
      break;
    }

    case MessageType::SEN_FILE_BLOCKNUM_REQ: {
      genSenFileReq messageContent = getMessageContent<genSenFileReq>(clientRequest);
      this->sendFileBlockNumber(clientSocket, messageContent);
      break;
    }

    case MessageType::SEN_FILE_BLOCK_REQ: {
      genSenFileReq messageContent = getMessageContent<genSenFileReq>(clientRequest);
      this->sendFileBlock(clientSocket, messageContent);
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

void SensorServer::sendFileNumber(int clientSocket, GenNumReq messageContent) {
  genMessage reply;
  fileNumberResp resp;
  std::vector<std::string> files = this->storage.listFiles();
  resp.totalFiles = static_cast<uint32_t>(files.size());
  reply.MID = static_cast<uint8_t>(MessageType::FILE_NUMBER_REP);
  reply.content = resp;
  this->listeningSocket.bSendData(clientSocket, reply);
  this->listeningSocket.closeSocket(clientSocket);
}

void SensorServer::sendFileNames(int clientSocket, GenNumReq messageContent) {
  genMessage reply;
  senFileNamesRes resp;
  std::vector<std::string> files = this->storage.listFiles();
  resp.page = 0;
  resp.totalPages = 1;
  for (std::string file : files){
    ++resp.page;  // ?
    if (file.size() >= 4 && file.rfind(".log") == file.size() - 4) {
      forNamesRequest nameReq;
      nameReq.Filename = file;
      resp.fileNames.names.push_back(nameReq);
      ++resp.totalPages;  // ?
    }
  }
  reply.MID = static_cast<uint8_t>(MessageType::SEN_FILE_NAMES_RES);
  reply.content = resp;
  this->listeningSocket.bSendData(clientSocket, reply);
  this->listeningSocket.closeSocket(clientSocket);
}

void SensorServer::sendSensorFileMetadata(int clientSocket, genSenFileReq messageContent) {
  genMessage reply;
  std::string filename = messageContent.fileName.Filename;
  iNode inode;
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
    reply.MID = static_cast<uint8_t>(MessageType::SEN_FILE_METD_RES);
    reply.content = res;
  } else {
    // mensaje de error
    errorCommonMsg err;
    err.message = "No se pudo obtener los metadatos del archivo " + filename;
    reply.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
    reply.content = err;
  }

  this->listeningSocket.bSendData(clientSocket, reply);
  this->listeningSocket.closeSocket(clientSocket);
}

void SensorServer::sendFileBlockNumber(int clientSocket, genSenFileReq messageContent) {
  genMessage reply;
  std::string fileName = messageContent.fileName.Filename;
  senFileBlockNumRes res;
  res.fileName = messageContent.fileName;
  uint32_t fsize = this->storage.getFileSize(fileName);
  if (fsize == 0) {
    res.blocks = 0;
  } else {
    res.blocks = (fsize + BLOCK_SIZE - 1) / BLOCK_SIZE;
  }

  reply.MID = static_cast<uint8_t>(MessageType::SEN_FILE_BLOCKNUM_RES);
  reply.content = res;
  this->listeningSocket.bSendData(clientSocket, reply);
  this->listeningSocket.closeSocket(clientSocket);
}

void SensorServer::sendFileBlock(int clientSocket, genSenFileReq messageContent) {
  genMessage reply;
  std::string fileName = messageContent.fileName.Filename;
  iNode inode;
  if (this->storage.getFileInfo(fileName, &inode)) {
    senFileBlockRes res;
    res.fileName = messageContent.fileName;
    res.usedBlocks = sizeof(inode.directBlocks) / sizeof(uint32_t);
    uint32_t blockSize = BLOCK_SIZE;
    char* firstBuffer;
    char* secondBuffer;
    if (this->storage.readFile(fileName, firstBuffer, blockSize)) {
      res.firstBlock = firstBuffer;
      if (this->storage.readFile(fileName, secondBuffer, blockSize)) {
        res.secondBlock = secondBuffer;
      }
    }

    res.page = 0;  // ???
    uint32_t totalPages = (inode.size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    res.totalPages = totalPages;
    reply.MID = static_cast<uint8_t>(MessageType::SEN_FILE_BLOCK_RESP);
    reply.content = res;
  } else {
    // mensaje de error
    errorCommonMsg err;
    err.message = "No se pudo obtener los metadatos del archivo " + fileName;
    reply.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
    reply.content = err;
  }

  this->listeningSocket.bSendData(clientSocket, reply);
  this->listeningSocket.closeSocket(clientSocket);
}