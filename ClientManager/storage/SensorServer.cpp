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
  this->listenForever(this->serverIP, this->listeningPort,
    "STORAGE_SERVER", DISC_STORAGE, ServerType::SV_STORAGE);
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
    std::cout << "Storage server error on start " << serverIP << ":" 
            << listeningPort << std::endl;
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
  
  std::cout << "\n========== SEND FILE BLOCK ==========" << std::endl;
  std::cout << "Requested file: '" << fileName << "'" << std::endl;
  
  // Obtener información del archivo
  this->storageMutex.lock();
  
  if (!this->storage.fileExists(fileName)) {
    this->storageMutex.unlock();
    std::cerr << "✗ File not found: " << fileName << std::endl;
    
    genMessage reply;
    errorCommonMsg err;
    err.message = "Archivo no encontrado";
    reply.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
    reply.content = err;
    this->listeningSocket.bSendData(clientSocket, reply);
    return;
  }
  
  uint32_t fileSize = this->storage.getFileSize(fileName);
  
  if (fileSize == 0) {
    this->storageMutex.unlock();
    std::cerr << "✗ File is empty" << std::endl;
    
    genMessage reply;
    errorCommonMsg err;
    err.message = "Archivo vacio";
    reply.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
    reply.content = err;
    this->listeningSocket.bSendData(clientSocket, reply);
    return;
  }
  
  std::cout << "File size: " << fileSize << " bytes" << std::endl;
  
  // Obtener el inodo del archivo
  iNode file;
  if (!this->storage.getFileInfo(fileName, &file)) {
    this->storageMutex.unlock();
    std::cerr << "✗ Could not get file info" << std::endl;
    
    genMessage reply;
    errorCommonMsg err;
    err.message = "Error al leer archivo";
    reply.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
    reply.content = err;
    this->listeningSocket.bSendData(clientSocket, reply);
    return;
  }
  
  // Calcular bloques necesarios
  uint32_t addedBlock = 0;
  if ((fileSize % BLOCK_SIZE) != 0) {
    addedBlock++;
  }
  
  uint32_t blockAmount = (fileSize / BLOCK_SIZE) + addedBlock;
  
  // Calcular páginas (2 bloques por página)
  uint32_t extraPage = 0;
  if ((blockAmount % 2) != 0) {
    extraPage++;
  }
  uint32_t totalPages = (blockAmount / 2) + extraPage;
  
  std::cout << "Total blocks: " << blockAmount << std::endl;
  std::cout << "Total pages: " << totalPages << std::endl;
  
  // Preparar buffers
  char* buffer1 = (char*) calloc(1024, sizeof(char));
  char* buffer2 = (char*) calloc(1024, sizeof(char));
  
  uint32_t sentBlocks = 0;
  uint32_t proccessedPages = 0;
  
  genMessage response;
  response.MID = static_cast<uint8_t>(MessageType::SEN_FILE_BLOCK_RESP);
  
  // Enviar bloques en pares
  while (sentBlocks < blockAmount) {
    senFileBlockRes responseContent;
    responseContent.id_token = messageContent.id_token;       // ← Del REQUEST
    responseContent.fileName = messageContent.fileName;       // ← Del REQUEST
    responseContent.page = proccessedPages;
    responseContent.totalPages = totalPages;
    responseContent.usedBlocks = 0;
    
    // Limpiar bloques
    responseContent.firstBlock.clear();
    responseContent.secondBlock.clear();
    memset(buffer1, '\0', 1024);
    memset(buffer2, '\0', 1024);
    
    // ===== PRIMER BLOQUE =====
    uint32_t blockNum = this->storage.getBlockNumber(file, sentBlocks);
    
    if (blockNum != BLOCK_FREE_SLOT) {
      // ACCESO DIRECTO como LogsServer
      this->storage.readBlock(blockNum, static_cast<void*>(buffer1));
      
      std::string text(buffer1, 1024);
      responseContent.firstBlock = text;
      sentBlocks++;
      proccessedPages++;
      
      std::cout << "  Page " << proccessedPages << "/" << totalPages 
                << ": Block1 read from physical block " << blockNum << std::endl;
    } else {
      std::cerr << "  ✗ Invalid block number for block " << sentBlocks << std::endl;
      break;
    }
    
    // ===== SEGUNDO BLOQUE (si existe) =====
    if (sentBlocks < blockAmount) {
      blockNum = this->storage.getBlockNumber(file, sentBlocks);
      
      if (blockNum != BLOCK_FREE_SLOT) {
        this->storage.readBlock(blockNum, static_cast<void*>(buffer2));
        
        std::string text(buffer2, 1024);
        responseContent.secondBlock = text;
        sentBlocks++;
        
        std::cout << "  Page " << proccessedPages << "/" << totalPages 
                  << ": Block2 read from physical block " << blockNum << std::endl;
      }
    }
    
    // Enviar página
    response.content = responseContent;
    
    this->storageMutex.unlock();  // Liberar mutex antes de enviar
    
    if (!this->listeningSocket.bSendData(clientSocket, response)) {
      std::cerr << "✗ Failed to send page " << proccessedPages << std::endl;
      free(buffer1);
      free(buffer2);
      return;
    }
    
    std::cout << "  ✓ Page " << proccessedPages << " sent successfully" << std::endl;
    
    this->storageMutex.lock();  // Volver a bloquear para la siguiente iteración
  }
  
  this->storageMutex.unlock();
  
  free(buffer1);
  free(buffer2);
  
  std::cout << "✓ Sent " << proccessedPages << " pages for file: " << fileName << std::endl;
  std::cout << "====================================\n" << std::endl;
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
      std::cout << "Removing expired sensor before sending: " << it->ip 
                << ", Type: " << it->sensorType << std::endl;
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
    std::cout << "  - Sensor " << (i+1) 
              << ": IP=" << this->recentData[i].ip 
              << ", Type=" << this->recentData[i].sensorType
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

void SensorServer::updateRecentSensorData(const std::string& sensorIP, 
                                          const std::string& sensorType,
                                          const std::string& data) {
  time_t currentTime = time(nullptr);
  bool found = false;
  
  // DEBUG: Mostrar lo que está llegando
  std::cout << "\n========== UPDATE RECENT SENSOR DATA ==========" << std::endl;
  std::cout << "INPUT - IP: '" << sensorIP << "' (length: " << sensorIP.length() << ")" << std::endl;
  std::cout << "INPUT - Type: '" << sensorType << "' (length: " << sensorType.length() << ")" << std::endl;
  std::cout << "INPUT - Data: '" << data << "' (length: " << data.length() << ")" << std::endl;
  
  // Mostrar en hexadecimal para detectar caracteres ocultos
  std::cout << "Type in HEX: ";
  for (char c : sensorType) {
    printf("%02X ", (unsigned char)c);
  }
  std::cout << std::endl;
  
  // Buscar si ya existe un registro para esta IP Y TIPO
  std::cout << "\nSearching in existing sensors (total: " << this->recentData.size() << ")..." << std::endl;
  
  for (size_t idx = 0; idx < this->recentData.size(); ++idx) {
    auto& sensor = this->recentData[idx];
    
    std::cout << "  [" << idx << "] Comparing with:" << std::endl;
    std::cout << "       Stored IP: '" << sensor.ip << "'" << std::endl;
    std::cout << "       Stored Type: '" << sensor.sensorType << "'" << std::endl;
    std::cout << "       Stored Data: '" << sensor.data << "'" << std::endl;
    
    bool ipMatch = (sensor.ip == sensorIP);
    bool typeMatch = (sensor.sensorType == sensorType);
    
    std::cout << "       IP Match: " << (ipMatch ? "YES" : "NO") << std::endl;
    std::cout << "       Type Match: " << (typeMatch ? "YES" : "NO") << std::endl;
    
    if (ipMatch && typeMatch) {
      // Actualizar datos existentes
      std::cout << "  ✓ MATCH FOUND! Updating sensor at index " << idx << std::endl;
      std::cout << "    Old data: '" << sensor.data << "'" << std::endl;
      std::cout << "    New data: '" << data << "'" << std::endl;
      
      sensor.data = data;
      sensor.lastModified = static_cast<uint32_t>(currentTime);
      found = true;
      
      std::cout << "  ✓ Updated successfully!" << std::endl;
      break;
    }
  }
  
  // Si no existe, agregar nuevo registro
  if (!found) {
    std::cout << "\n✓ NO MATCH FOUND - Adding NEW sensor" << std::endl;
    
    sensorRecentData newSensor;
    newSensor.ip = sensorIP;
    newSensor.sensorType = sensorType;
    newSensor.data = data;
    newSensor.lastModified = static_cast<uint32_t>(currentTime);
    
    this->recentData.push_back(newSensor);
    
    std::cout << "  New sensor added:" << std::endl;
    std::cout << "    IP: '" << newSensor.ip << "'" << std::endl;
    std::cout << "    Type: '" << newSensor.sensorType << "'" << std::endl;
    std::cout << "    Data: '" << newSensor.data << "'" << std::endl;
    std::cout << "  Vector size is now: " << this->recentData.size() << std::endl;
  }
  
  // Limpiar sensores que han excedido el tiempo de olvido
  std::cout << "\nCleaning expired sensors..." << std::endl;
  auto it = this->recentData.begin();
  size_t removedCount = 0;
  
  while (it != this->recentData.end()) {
    double elapsedTime = difftime(currentTime, it->lastModified);
    
    if (elapsedTime > SENSOR_FORGET_TIME) {
      std::cout << "  ✗ Removing expired sensor: IP=" << it->ip 
                << ", Type='" << it->sensorType << "'"
                << " (inactive for " << elapsedTime << " seconds)" << std::endl;
      it = this->recentData.erase(it);
      removedCount++;
    } else {
      ++it;
    }
  }
  
  std::cout << "Removed " << removedCount << " expired sensor(s)" << std::endl;
  
  // Mostrar estado final del vector
  std::cout << "\n========== FINAL STATE ==========" << std::endl;
  std::cout << "Total active sensors: " << this->recentData.size() << std::endl;
  for (size_t i = 0; i < this->recentData.size(); ++i) {
    std::cout << "  [" << i << "] IP='" << this->recentData[i].ip 
              << "', Type='" << this->recentData[i].sensorType 
              << "', Data='" << this->recentData[i].data << "'" << std::endl;
  }
  std::cout << "================================\n" << std::endl;
}

void SensorServer::addToSensorLog(senAddLog& messageContent) {
  std::lock_guard<std::mutex> lock(this->storageMutex);

  std::cout << "\n========== ADD TO SENSOR LOG ==========" << std::endl;
  std::cout << "Received senAddLog message:" << std::endl;
  std::cout << "  Origin IP: '" << messageContent.originIP << "'" << std::endl;
  std::cout << "  Sensor Type: '" << messageContent.fileName.sensorType << "'" << std::endl;
  std::cout << "  Data: '" << messageContent.data << "'" << std::endl;
  
  // Mostrar el tipo en hexadecimal
  std::cout << "  Type (HEX): ";
  for (char c : messageContent.fileName.sensorType) {
    printf("%02X ", (unsigned char)c);
  }
  std::cout << std::endl;

  // Actualizar datos recientes ANTES de guardar en archivo
  std::cout << "\nCalling updateRecentSensorData..." << std::endl;
  this->updateRecentSensorData(messageContent.originIP, 
                                messageContent.fileName.sensorType,
                                messageContent.data);
  
  std::string fileName = this->getSensorFileName(messageContent.fileName);
  std::cout << "\nSaving to file: " << fileName << std::endl;
  
  // Obtener tamaño actual del archivo
  uint32_t sizeBeforeWrite = 0;
  if (this->storage.fileExists(fileName)) {
    sizeBeforeWrite = this->storage.getFileSize(fileName);
    std::cout << "File exists, current size: " << sizeBeforeWrite << " bytes" << std::endl;
  } else {
    std::cout << "File doesn't exist, creating..." << std::endl;
    this->storage.createFile(fileName);
  }
  
  // Agregar datos del sensor
  uint32_t dataSize = messageContent.data.size();
  this->storage.appendFile(fileName, messageContent.data.data(), dataSize);
  
  // Add newline for better formatting
  std::string newline = "\n";
  this->storage.appendFile(fileName, newline.data(), newline.size());
  
  // Calcular nuevo tamaño
  uint32_t newSize = sizeBeforeWrite + dataSize + newline.size();
  std::cout << "✓ Added " << (dataSize + newline.size()) << " bytes to " << fileName 
            << " (size: " << sizeBeforeWrite << " → " << newSize << ")" << std::endl;
  
  // Leer últimas líneas del archivo para mostrar
  const uint32_t READ_SIZE = 1024;
  char buffer[READ_SIZE];
  memset(buffer, 0, READ_SIZE);
  
  uint32_t totalFileSize = this->storage.getFileSize(fileName);
  uint32_t readSize = std::min(totalFileSize, READ_SIZE - 1);
  
  this->storage.readFile(fileName, buffer, readSize);
  buffer[readSize] = '\0';
  
  std::cout << "\nFile contents (last " << readSize << " bytes):" << std::endl;
  std::cout << "--- BEGIN ---" << std::endl;
  std::cout << buffer << std::endl;
  std::cout << "--- END ---" << std::endl;
  std::cout << "======================================\n" << std::endl;
}
