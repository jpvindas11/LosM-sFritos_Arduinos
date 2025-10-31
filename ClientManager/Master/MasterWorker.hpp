#ifndef MASTERWORKER_HPP
#define MASTERWORKER_HPP

#include <string>
#include "../Util/Socket.hpp"
#include "../Util/IPConstants.hpp"
#include "../Util/Thread.hpp"

class MasterServer;

class MasterWorker : Thread {
 friend MasterServer;

 private:
  int clientSocket;
  std::string serverIP;
  int serverPort;
  genMessage clientRequest;

 public:
  MasterWorker(int clientSocket, const std::string& serverIP, 
               int serverPort, const genMessage& request)
      : clientSocket(clientSocket), serverIP(serverIP), 
        serverPort(serverPort), clientRequest(request) {
  }

 protected:
  int run() override {
    Socket backendSocket;
    backendSocket.create();
    if (!backendSocket.connectToServer(serverIP, serverPort)) {
      std::cerr << "Failed to connect to backend" << std::endl;
      close(clientSocket);
      return EXIT_FAILURE;
    }

    backendSocket.bSendData(backendSocket.getSocketFD(), clientRequest);

    // Get response from backend
    genMessage response;
    backendSocket.bReceiveData(backendSocket.getSocketFD(), response);

    Socket clientReq;
    clientReq.bSendData(clientSocket, response);

    backendSocket.closeSocket();
    close(clientSocket);

    return EXIT_SUCCESS;
  }
};

#endif