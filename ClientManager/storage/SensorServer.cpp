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

    case MessageType::ADD_SENSOR: {
      addSensor messageContent = getMessageContent<genSenFileReq>(clientRequest);
      // this->addSensor(clientSocket, messageContent);
      break;
    }

    case MessageType::DELETE_SENSOR: {
      deleteSensor messageContent = getMessageContent<genSenFileReq>(clientRequest);
      // this->deleteSensor(clientSocket, messageContent);
      break;
    }

    case MessageType::MODIFY_SENSOR: {
      modifySensorInfp messageContent = getMessageContent<genSenFileReq>(clientRequest);
      // this->modifySensor(clientSocket, messageContent);
      break;
    }

    default: {
      std::cerr << "ERROR: MID non recognized" << std::endl;
      break;
    }
  }
}

void SensorServer::addToSensorLog(senAddLog& messageContent) {
  std::lock_guard<std::mutex> lock(storageMutex); // Thread-safe file access
  
  std::string fileName = this->getSensorFileName(messageContent.fileName);
  std::cout << "Adding to sensor log: " << fileName << std::endl;
  
  if (!this->storage.fileExists(fileName)) {
    this->storage.createFile(fileName);
  }
  this->storage.appendFile(fileName, messageContent.data.data()
                                   , messageContent.data.size());
  
  // Add newline for better formatting
  std::string newline = "\n";
  this->storage.appendFile(fileName, newline.data(), newline.size());
  
  char buffer [256];
  uint32_t size = 256;
  this->storage.readFile(fileName, buffer, size);
  std::cout << "Received for file -> " << fileName << "\n" << buffer<<std::endl;
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
  return name.sensorType + "_" + idS + "_" + yearS + "/"
       + monthS + "/" + dayS + ".log";
}

void SensorServer::sendFileNumber(int clientSocket, GenNumReq messageContent) {
  genMessage reply;
  fileNumberResp resp;
  resp.id_token = messageContent.id_token;
  std::vector<std::string> files = this->storage.listFiles();
  resp.totalFiles = static_cast<uint32_t>(files.size());
  reply.MID = static_cast<uint8_t>(MessageType::FILE_NUMBER_REP);
  reply.content = resp;
  this->listeningSocket.bSendData(clientSocket, reply);
}

void SensorServer::sendFileNames(int clientSocket, GenNumReq messageContent) {
  genMessage reply;
  senFileNamesRes resp;
  resp.id_token = messageContent.id_token;
  std::vector<std::string> files = this->storage.listFiles();
  resp.page = 0;
  resp.totalPages = 1;
  for (std::string file : files){
    if (file.size() >= 4 && file.rfind(".log") == file.size() - 4) {
      forNamesRequest nameReq;
      nameReq.Filename = file;
      resp.fileNames.names.push_back(nameReq);
    }
  }
  reply.MID = static_cast<uint8_t>(MessageType::SEN_FILE_NAMES_RES);
  reply.content = resp;
  this->listeningSocket.bSendData(clientSocket, reply);
}

void SensorServer::sendSensorFileMetadata(int clientSocket, genSenFileReq messageContent) {
  genMessage reply;
  std::string filename = messageContent.fileName.Filename;
  iNode inode;
  if (this->storage.getFileInfo(filename, &inode)) {
    senFileMetDRes res;
    res.id_token = messageContent.id_token;
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
}

void SensorServer::sendFileBlockNumber(int clientSocket, genSenFileReq messageContent) {
  genMessage reply;
  std::string fileName = messageContent.fileName.Filename;
  res.id_token = messageContent.id_token;
  res.fileName = messageContent.fileName;
  iNode inode;
  if (this->storage.getFileInfo(fileName, &inode)) {
    senFileBlockNumRes res;
    res.id_token = messageContent.id_token;
    res.fileName = messageContent.fileName;
    res.blocks = 0;
    for (size_t idx = 0; idx < TOTAL_DIRECT_POINTERS; ++idx) {
      if (inode.directBlocks[idx] != BLOCK_FREE_SLOT) {
        ++res.blocks;
      }
    }
    reply.MID = static_cast<uint8_t>(MessageType::SEN_FILE_BLOCKNUM_RES);
    reply.content = res;
  } else {
    // mensaje de error
    errorCommonMsg err;
    err.message = "No se pudo obtener los metadatos del archivo " + fileName;
    reply.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
    reply.content = err;
  }

  reply.MID = static_cast<uint8_t>(MessageType::SEN_FILE_BLOCKNUM_RES);
  reply.content = res;
  this->listeningSocket.bSendData(clientSocket, reply);
}

void SensorServer::sendFileBlock(int clientSocket, genSenFileReq messageContent) {
  std::string fileName = messageContent.fileName.Filename;
  iNode inode;
  // Trata de leer los datos de archivo
  if (this->storage.getFileInfo(fileName, &inode)) {
    u_int8_t totalBlocks = 0;
    for (size_t idx = 0; idx < TOTAL_DIRECT_POINTERS; ++idx) {
      if (inode.directBlocks[idx] != BLOCK_FREE_SLOT) {
        ++totalBlocks;
      }
    }
    uint32_t blockSize = BLOCK_SIZE;
    uint32_t localPage = 0;
    for (int i = 0; i < totalBlocks; i += 2) {
      // Crea mensaje genérico y de respuesta
      genMessage reply;
      senFileBlockRes res;
      res.id_token = messageContent.id_token;
      res.fileName = messageContent.fileName;
      res.usedBlocks = totalBlocks;
      res.page = localPage++;
      res.totalPages = (totalBlocks + 1) / 2;
      // Lee bloque 1 desde buffer
      // No hay manera de leer a partir de cierto punto?
      // como se que buffer1 y 2 no serán iguales?
      char* buffer1 = new char[blockSize];
      if (this->storage.readFile(fileName, buffer1, blockSize)) {
        std::string firstBlock(buffer1);
        res.firstBlock = firstBlock;
      }
      delete[] buffer1;
      // Lee bloque 2 desde buffer (si existe)
      if (i + 1 < 2) {
        char* buffer2 = new char[blockSize];
        if (this->storage.readFile(fileName, buffer2, blockSize)) {
          std::string secondBlock(buffer2);
          res.secondBlock = secondBlock;
        }
        delete[] buffer2;
      }
      // Envía la respuesta
      reply.MID = static_cast<uint8_t>(MessageType::SEN_FILE_BLOCK_RESP);
      reply.content = res;
      this->listeningSocket.bSendData(clientSocket, reply);
      }
  } else {
    // mensaje de error
    errorCommonMsg err;
    err.message = "No se pudo obtener los metadatos del archivo " + fileName;
    reply.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
    reply.content = err;
  }
}
