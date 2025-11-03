#ifndef MASTERWORKER_HPP
#define MASTERWORKER_HPP

#include <string>
#include <sys/socket.h>
#include <unistd.h>
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
    std::cout << "[WORKER] Processing request for client " << clientSocket << std::endl;
    
    Socket backendSocket;
    if (!backendSocket.create()) {
      std::cerr << "[WORKER] Failed to create backend socket" << std::endl;
      close(clientSocket);
      return EXIT_FAILURE;
    }
    
    if (!backendSocket.connectToServer(serverIP, serverPort)) {
      std::cerr << "[WORKER] Failed to connect to backend at " 
                << serverIP << ":" << serverPort << std::endl;
      backendSocket.closeSocket();
      close(clientSocket);
      return EXIT_FAILURE;
    }

    std::cout << "[WORKER] Connected to backend, sending request..." << std::endl;
    
    // Enviar request al backend
    ssize_t sent = backendSocket.bSendData(backendSocket.getSocketFD(), clientRequest);
    if (sent <= 0) {
      std::cerr << "[WORKER] Failed to send data to backend" << std::endl;
      backendSocket.closeSocket();
      close(clientSocket);
      return EXIT_FAILURE;
    }

    std::cout << "[WORKER] Request sent to backend, waiting for response..." << std::endl;

    // Recibir respuesta del backend
    genMessage response;
    ssize_t received = backendSocket.bReceiveData(backendSocket.getSocketFD(), response);
    if (received <= 0) {
      std::cerr << "[WORKER] Failed to receive response from backend" << std::endl;
      backendSocket.closeSocket();
      close(clientSocket);
      return EXIT_FAILURE;
    }

    std::cout << "[WORKER] Response received from backend, sending to client..." << std::endl;

    // Enviar respuesta al cliente usando send directamente
    // Primero el tamaño
    std::vector<uint8_t> buffer;
    buffer.reserve(1024);
    bitsery::Serializer<bitsery::OutputBufferAdapter<std::vector<uint8_t>>> ser(buffer);
    ser.object(response);
    ser.adapter().flush();

    uint32_t messageSize = static_cast<uint32_t>(buffer.size());
    
    if (send(clientSocket, &messageSize, sizeof(messageSize), 0) <= 0) {
      std::cerr << "[WORKER] Failed to send message size to client" << std::endl;
      backendSocket.closeSocket();
      close(clientSocket);
      return EXIT_FAILURE;
    }

    // Luego el mensaje
    if (send(clientSocket, buffer.data(), buffer.size(), 0) <= 0) {
      std::cerr << "[WORKER] Failed to send message to client" << std::endl;
      backendSocket.closeSocket();
      close(clientSocket);
      return EXIT_FAILURE;
    }

    std::cout << "[WORKER] Response sent to client successfully" << std::endl;

    backendSocket.closeSocket();
    close(clientSocket);

    return EXIT_SUCCESS;
  }
};

#endif