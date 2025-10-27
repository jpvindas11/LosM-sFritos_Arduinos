#include "SensorServer.hpp"

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
  std::string fileName = this->getSensorFileName(messageContent.fileName);
  if (!this->storage.fileExists(fileName)) {
    this->storage.createFile(fileName);
  }
  this->storage.appendFile(fileName, messageContent.data.data(), 
                                                    messageContent.data.size());
}

std::string getSensorFileName(sensorFileName& name) {
  std::stringstream ss;
  ss<<name.sensorType<<"_"<<name.id<<"_"<<name.year<<name.month<<name.day;
  return ss.str();
}