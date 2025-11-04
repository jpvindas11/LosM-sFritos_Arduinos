#include "LogsServer.hpp"

LogsServer::LogsServer() : running(false) {
}

LogsServer& LogsServer::getInstance() {
  static LogsServer server;
  return server;
}

int LogsServer::startServer(std::string serverIP, int listeningPort) {
  this->serverIP = serverIP;
  this->listeningPort = listeningPort;
  if (!this->storage.mount("userLogsStorage.bin")) {
    return EXIT_FAILURE;
  }
  this->listenForever(this->serverIP, this->listeningPort);
  return EXIT_SUCCESS;
}

void LogsServer::stopServer() {  
  running.store(false);
  this->storage.unmount();
}

void LogsServer::closeListeningSocket() {
  this->listeningSocket.closeSocket();
}

void LogsServer::run(std::string serverIP, int listeningPort){
  try{
    if (this->startServer(serverIP, listeningPort) == EXIT_SUCCESS) {
      running.store(true);
      std::cout << "Logs server started on " << serverIP << ":" 
                << listeningPort << " (multi-threaded mode)\n";
      this->acceptAllConnections();
    }
  } catch(const std::runtime_error& error) {
    std::cerr<<error.what()<<std::endl;
  }
  this->stopServer();
}

void LogsServer::handleClientConnection(int clientSocket) {
  // Crear un thread separado para manejar cada cliente
  std::thread clientThread(&LogsServer::processClientInThread, this, clientSocket);
  clientThread.detach(); // Permitir que el thread se ejecute independientemente
}

void LogsServer::processClientInThread(int clientSocket) {
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
  this->storageMutex.lock();
    this->storage.inspectDisk();
  this->storageMutex.unlock();
  
  // Cerrar el socket del cliente
  std::cout << "Closing client socket: " << clientSocket << std::endl;
  this->listeningSocket.closeSocket(clientSocket);
}

void LogsServer:: serveClient(int clientSocket, genMessage& clientRequest) {
  switch (static_cast<MessageType>(clientRequest.MID)) {
    
    case MessageType::LOG_USER_REQUEST: {
      userLogRequestCommon messageContent = getMessageContent<userLogRequestCommon>(clientRequest);
      this->sendUserLogs(clientSocket, messageContent);
      break;
    }

    case MessageType::ADD_USER_LOG: {
      addUserLog messageContent = getMessageContent<addUserLog>(clientRequest);
      std::cout<<messageContent.logInfo<<std::endl;
      this->addUserLogProcc(messageContent);
      break;
    }

    case MessageType::DELETE_USER_LOGS: {
      userLogRequestCommon messageContent = getMessageContent<userLogRequestCommon>(clientRequest);
      this->deleteUser(messageContent);
      break;
    }

    default: {
      std::cerr << "ERROR: MID non recognized" << std::endl;
      break;
    }
  }
}

void LogsServer::sendUserLogs(int clientSocket, userLogRequestCommon& message) {
  this->storageMutex.lock();
  if (!this->storage.fileExists(message.userName)){
    this->storageMutex.unlock();
    this->sendErrorMessage(clientSocket,
                                 "ERROR: USER LOG FILE IS NOT IN THE DATABASE");
    return;
  }
  uint32_t fileSize = this->storage.getFileSize(message.userName);
  this->storageMutex.unlock();
  if (fileSize == 0) {
    this->sendErrorMessage(clientSocket,"ERROR: EMPTY FILE");
    return;
  }

  uint32_t addedBlock = 0;

  if ((fileSize % BLOCK_SIZE) != 0) {
    addedBlock++;
  }

  genMessage response;

  uint32_t extraPage = 0;

  userLogResp messageContent;

  uint32_t blockAmount = (fileSize / BLOCK_SIZE) + addedBlock;

  if ((blockAmount % 2) != 0) {
    extraPage++;
  }

  messageContent.id_token = message.id_token;
  messageContent.userName = message.userName;
  messageContent.totalPages =
                           (blockAmount / static_cast<uint32_t>(2)) + extraPage;

  uint32_t sentBlocks = 0;
  char* buffer1 = (char*) calloc(1024, sizeof(char));
  char* buffer2 = (char*) calloc(1024, sizeof(char));
  iNode file;

  this->storageMutex.lock();
    this->storage.getFileInfo(message.userName, &file);
  this->storageMutex.unlock();

  uint32_t proccessedPages = 0;

  response.MID = static_cast<uint8_t>(MessageType::LOG_USER_RESP);

  while (sentBlocks < blockAmount) {
    messageContent.firstBlock.clear();
    messageContent.secondBlock.clear();
    this->storageMutex.lock();
      uint32_t blockNum = this->storage.getBlockNumber(file, sentBlocks);
    this->storageMutex.unlock();
    if (blockNum != BLOCK_FREE_SLOT) {
      this->storageMutex.lock();
        this->storage.readBlock(blockNum, static_cast<void*>(buffer1));
      this->storageMutex.unlock();
      std::string text(buffer1, 1024);
      messageContent.firstBlock = text;
      sentBlocks++;
      proccessedPages++;
      messageContent.page = proccessedPages;
    } else {
      break;
    }
    this->storageMutex.lock();
      blockNum = this->storage.getBlockNumber(file, sentBlocks);
    this->storageMutex.unlock();
    if (blockNum != BLOCK_FREE_SLOT) {
      this->storageMutex.lock();
        this->storage.readBlock(blockNum, static_cast<void*>(buffer2));
      this->storageMutex.unlock();
      std::string text(buffer2, 1024);
      messageContent.secondBlock = text;
      sentBlocks++;
    }
    response.content = messageContent;
    this->listeningSocket.bSendData(clientSocket, response);
    memset(buffer1, '\0', 1024);
    memset(buffer2, '\0', 1024);
  }
  free(buffer1);
  free(buffer2);
}

void LogsServer:: addUserLogProcc(addUserLog& message) {
  this->storageMutex.lock();
    if (!this->storage.fileExists(message.userName)) {
      this->storage.createFile(message.userName);
    }
    this->storage.appendFile(message.userName, message.logInfo.data(),
                                                        message.logInfo.size());
    this->storage.printFileDetails(message.userName);
  this->storageMutex.unlock();
}

void LogsServer:: deleteUser(userLogRequestCommon& message) {
  this->storageMutex.lock();
    if (!this->storage.fileExists(message.userName)) {
      this->storageMutex.unlock();
      return;
    }
    this->storage.deleteFile(message.userName);
  this->storageMutex.unlock();
}

void LogsServer::sendErrorMessage(int clientSocket, const std::string& error) {
  genMessage message;
  errorCommonMsg errorContent;
  errorContent.message = error;
  message.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
  message.content = errorContent;
  this->listeningSocket.bSendData(clientSocket, message);
}