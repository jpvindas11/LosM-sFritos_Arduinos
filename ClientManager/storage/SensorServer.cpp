#include "SensorServer.hpp"
#include <algorithm>

SensorServer::SensorServer() : running(false) {
}

SensorServer& SensorServer::getInstance() {
  static SensorServer server;
  return server;
}

int SensorServer::startServer(std::string serverIP, int listeningPort) {
  this->serverIP = serverIP;
  this->listeningPort = listeningPort;
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

void SensorServer::run(std::string serverIP, int listeningPort){
  try{
    if (this->startServer(serverIP, listeningPort) == EXIT_SUCCESS) {
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

    case MessageType::SEN_RECENT_DATA_REQ: {
      GenNumReq messageContent = getMessageContent<GenNumReq>(clientRequest);
      this->sendRecentData(clientSocket, messageContent);
      break;
    }

    case MessageType::ADD_SENSOR: {
      addSensor messageContent = getMessageContent<addSensor>(clientRequest);
      this->addToSensorServer(messageContent);
      break;
    }

    case MessageType::DELETE_SENSOR: {
      deleteSensor messageContent = getMessageContent<deleteSensor>(clientRequest);
      this->deleteFromSensorServer(messageContent);
      break;
    }

    case MessageType::MODIFY_SENSOR: {
      modifySensorInfp messageContent = getMessageContent<modifySensorInfp>(clientRequest);
      this->modifySensor(messageContent);
      break;
    }

    default: {
      std::cerr << "ERROR: MID non recognized" << std::endl;
      break;
    }
  }
}

void SensorServer::addToSensorLog(senAddLog& messageContent) {
  std::lock_guard<std::mutex> lock(this->storageMutex); // Thread-safe file access

  this->updateRecentSensorData(messageContent.originIP, messageContent.data);
  
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

void SensorServer::sendFileNumber(int clientSocket, GenNumReq& messageContent) {
  genMessage reply;
  fileNumberResp resp;
  resp.id_token = messageContent.id_token;
  std::vector<std::string> files = this->storage.listFiles();
  resp.totalFiles = static_cast<uint32_t>(files.size());
  reply.MID = static_cast<uint8_t>(MessageType::FILE_NUMBER_REP);
  reply.content = resp;
  this->listeningSocket.bSendData(clientSocket, reply);
}

void SensorServer::sendFileNames(int clientSocket, GenNumReq& messageContent) {
  std::vector<std::string> files = this->storage.listFiles();
  std::vector<std::string> logFiles;
  // Busca solo los archivos .log
  for (const std::string& file : files) {
    if (file.size() >= 4 && file.rfind(".log") == file.size() - 4) {
      logFiles.push_back(file);
    }
  }
  // Define la cantidad de mensajes a enviar
  size_t pageSize = 50;
  uint32_t actualPage = 0;
  uint32_t totalPages = (logFiles.size() + pageSize - 1) / pageSize;
  // Reparte el contenido entre los mensajes
  for (size_t idx = 0; idx < totalPages; ++idx) {
    // Crea el mensaje y la respuesta
    genMessage reply;
    senFileNamesRes resp;
    resp.id_token = messageContent.id_token;
    resp.page = actualPage++;
    resp.totalPages = totalPages;
    // Divide el contenido
    size_t inicio = idx * pageSize;
    size_t fin = std::min(inicio + pageSize, logFiles.size());
    
    // Convierte cada string a forNamesRequest
    std::vector<forNamesRequest> subvector;
    for (size_t i = inicio; i < fin; ++i) {
      forNamesRequest nameReq;
      nameReq.Filename = logFiles[i];
      subvector.push_back(nameReq);
    }
    
    resp.fileNames.names = subvector;
    // Envía el mensaje
    reply.MID = static_cast<uint8_t>(MessageType::SEN_FILE_NAMES_RES);
    reply.content = resp;
    this->listeningSocket.bSendData(clientSocket, reply);
  }
}

void SensorServer::sendSensorFileMetadata(int clientSocket, genSenFileReq& messageContent) {
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

void SensorServer::sendFileBlockNumber(int clientSocket, genSenFileReq& messageContent) {
  genMessage reply;
  std::string fileName = messageContent.fileName.Filename;
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
  
  this->listeningSocket.bSendData(clientSocket, reply);
}

void SensorServer::sendFileBlock(int clientSocket, genSenFileReq& messageContent) {
  std::string fileName = messageContent.fileName.Filename;
  iNode inode;
  
  // Trata de leer los datos de archivo
  if (this->storage.getFileInfo(fileName, &inode)) {
    uint8_t totalBlocks = 0;
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
      char buffer1[blockSize];
      memset(buffer1, 0, blockSize);
      uint32_t readSize = blockSize;
      if (this->storage.readFile(fileName, blockSize * i, buffer1, readSize)) {
        res.firstBlock = std::string(buffer1, readSize);
      }
      
      // Lee bloque 2 desde buffer (si existe)
      if (i + 1 < totalBlocks) {
        char buffer2[blockSize];
        memset(buffer2, 0, blockSize);
        readSize = blockSize;
        if (this->storage.readFile(fileName, blockSize * (i + 1), buffer2, readSize)) {
          res.secondBlock = std::string(buffer2, readSize);
        }
      }
      
      // Envía la respuesta
      reply.MID = static_cast<uint8_t>(MessageType::SEN_FILE_BLOCK_RESP);
      reply.content = res;
      this->listeningSocket.bSendData(clientSocket, reply);
    }
  } else {
    // mensaje de error
    genMessage reply;
    errorCommonMsg err;
    err.message = "No se pudo obtener los metadatos del archivo " + fileName;
    reply.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
    reply.content = err;
    this->listeningSocket.bSendData(clientSocket, reply);
  }
}

void SensorServer::addToSensorServer(addSensor& messageContent) {
  std::lock_guard<std::mutex> lock(this->storageMutex);

  std::ostringstream fileName_ss;
  fileName_ss << messageContent.name << "_"
              << messageContent.id << ".txt";

  std::string fileName = fileName_ss.str();
  // Si el archivo ya existe no hace nada
  if (!this->storage.fileExists(fileName)) {

    if (!this->storage.createFile(fileName)) {
      std::cerr << "Failed to create file -> "
                << fileName
                << "\n"
                << std::endl;
      return;
    }
    
    std::ostringstream content_ss;
    content_ss << messageContent.id_token << "_"
               << messageContent.name << "_"
               << messageContent.id << "_"
               << messageContent.state << "_"
               << messageContent.addition_year << "_"
               << messageContent.addition_moth << "_"
               << messageContent.addition_day << "_"
               << messageContent.last_send_year << "_"
               << messageContent.last_send_moth << "_"
               << messageContent.last_send_day << "_"
               << messageContent.added_by << "\n";
    std::string content = content_ss.str();

    if (!this->storage.appendFile(fileName, content.data(), content.size())) {
      std::cerr << "Failed to write on file -> "
                << fileName
                << "\n"
                << std::endl;
      return;
    }

    // debug
    char buffer [256];
    uint32_t size = 256;
    this->storage.readFile(fileName, buffer, size);
    std::cout << "Received for file -> "
              << fileName
              << "\n"
              << buffer
              << std::endl;
  }
}

void SensorServer::deleteFromSensorServer(deleteSensor& messageContent) {
  std::lock_guard<std::mutex> lock(this->storageMutex);

  std::string fileName = messageContent.name;
  // Si el archivo no existe envía un error
  if (this->storage.fileExists(fileName)) {
    if (!this->storage.deleteFile(fileName)) {
      std::cerr << "Failed to delete file -> "
                << fileName
                << "\n"
                << std::endl;
      return;
    }
    std::cout << "Deleted sensor -> "
              << fileName
              << "\n"
              << std::endl;
    if (messageContent.delete_logs) {
      size_t pos = fileName.rfind(".txt");
      fileName.replace(pos, 4, ".log");
      if (!this->storage.deleteFile(fileName)) {
        std::cerr << "Failed to delete log file -> "
                  << fileName
                  << "\n"
                  << std::endl;
        return;
      }
      std::cout << "Deleted log -> "
                << fileName
                << "\n"
                << std::endl;
    }
  } else {
    std::cerr << "File -> "
              << fileName
              << " doesn't exist"
              << "\n"
              << std::endl;
  }
}

void SensorServer::modifySensor(modifySensorInfp& messageContent) {
  std::lock_guard<std::mutex> lock(this->storageMutex);

  std::string fileName = messageContent.name;
  // Si el archivo no existe envía un error
  if (this->storage.fileExists(fileName)) {
    char buffer [1024];
    uint32_t size = 1024;
    if (this->storage.readFile(fileName, buffer, size)) {
      // extrae los datos guardados
      std::string ogContent(buffer);
      std::vector<std::string> parts;
      std::stringstream ss(ogContent);
      std::string item;
      while (std::getline(ss, item, '_')) {
        parts.push_back(item);
      }
      // modifica los datos
      parts[7] = std::to_string(messageContent.last_send_year);
      parts[8] = std::to_string(messageContent.last_send_moth);
      parts[9] = std::to_string(messageContent.last_send_day);
      // cambia el estado del sensor
      if (messageContent.modifyState) {
        parts[3] = std::to_string(messageContent.newState);
      }
      // guarda los datos como strings
      std::ostringstream oss;
      for (size_t i = 0; i < parts.size(); ++i) {
        oss << parts[i];
        if (i + 1 < parts.size()) {
          oss << '_';
        }
      }
      std::string newContent = oss.str();
      // reescribe los datos modificados
      if (!this->storage.writeFile(fileName, newContent.data()
                                           , newContent.size())) {
        std::cerr << "Failed to write modified metadata to "
                  << fileName
                  << std::endl;
        return;
      }
      std::cout << "Modified sensor file-> "
                << fileName 
                << "\n"
                << std::endl;
    }
  } else {
    std::cerr << "File -> "
              << fileName
              << " doesn't exist"
              << "\n"
              << std::endl;
  }
}

void SensorServer::sendRecentData(int clientSocket, GenNumReq& messageContent) {
  std::lock_guard<std::mutex> lock(this->storageMutex);
  
  std::cout << "Sending recent sensor data (total sensors: " << this->recentData.size() << ")" << std::endl;
  
  time_t currentTime = time(nullptr);
  auto it = this->recentData.begin();
  while (it != this->recentData.end()) {
    double elapsedTime = difftime(currentTime, it->lastModified);
    if (elapsedTime > SENSOR_FORGET_TIME) {
      std::cout << "Removing expired sensor before sending: " << it->ip << std::endl;
      it = this->recentData.erase(it);
    } else {
      ++it;
    }
  }
  
  genMessage reply;
  senRecentDataRes response;
  
  size_t maxSensors = std::min(this->recentData.size(), size_t(20));
  response.recentData.reserve(maxSensors);
  
  for (size_t i = 0; i < maxSensors; ++i) {
    response.recentData.push_back(this->recentData[i]);
    std::cout << "  - Sensor " << (i+1) << ": IP=" << this->recentData[i].ip 
              << ", Data=" << this->recentData[i].data 
              << ", Age=" << (currentTime - this->recentData[i].lastModified) << "s"
              << std::endl;
  }
  
  reply.MID = static_cast<uint8_t>(MessageType::SEN_RECENT_DATA_RES);
  reply.content = response;
  
  if (this->listeningSocket.bSendData(clientSocket, reply)) {
    std::cout << "Recent data sent successfully (" 
              << response.recentData.size() << " sensors)" << std::endl;
  } else {
    std::cerr << "ERROR: Failed to send recent data" << std::endl;
  }
}

void SensorServer::updateRecentSensorData(const std::string& sensorIP, const std::string& data) {
  time_t currentTime = time(nullptr);
  bool found = false;
  
  // Buscar si ya existe un registro para esta IP
  for (auto& sensor : this->recentData) {
    if (sensor.ip == sensorIP) {
      // Actualizar datos existentes
      sensor.data = data;
      sensor.lastModified = static_cast<uint32_t>(currentTime);
      found = true;
      std::cout << "Updated recent data for sensor IP: " << sensorIP << std::endl;
      break;
    }
  }
  
  // Si no existe, agregar nuevo registro
  if (!found) {
    sensorRecentData newSensor;
    newSensor.ip = sensorIP;
    newSensor.data = data;
    newSensor.lastModified = static_cast<uint32_t>(currentTime);
    this->recentData.push_back(newSensor);
    std::cout << "Added new sensor to recent data: " << sensorIP << std::endl;
  }
  
  // Limpiar sensores que han excedido el tiempo de olvido
  auto it = this->recentData.begin();
  while (it != this->recentData.end()) {
    double elapsedTime = difftime(currentTime, it->lastModified);
    
    if (elapsedTime > SENSOR_FORGET_TIME) {
      std::cout << "Removing expired sensor from recent data: " << it->ip 
                << " (inactive for " << elapsedTime << " seconds)" << std::endl;
      it = this->recentData.erase(it);
    } else {
      ++it;
    }
  }
  
  // Mostrar estado actual del vector
  std::cout << "Current active sensors in memory: " << this->recentData.size() << std::endl;
}
