#ifndef SENSORSERVER_HPP
#define SENSORSERVER_HPP

#include <sstream>
#include <cstdio>

#include "StorageServer.hpp"


class SensorServer : public StorageServer {
 private:
  /* data */
 public:
  static SensorServer& getInstance();
  void run(std::string serverIP, int listeningPort, 
                                          std::string masterIP, int materPort);
  void stopServer();
  void handleClientConnection(int  clientSocket) override;
  void serveClient(int clientSocket, genMessage& clientRequest);
  void addToSensorLog(senAddLog& messageContent);
  std::string getSensorFileName(sensorFileName& name);
  void closeListeningSocket();
  std::string getFromBuffer(char* buffer, uint32_t size);

 private:
  int startServer(std::string serverIP, int listeningPort, 
                                          std::string masterIP, int materPort);
  SensorServer();
  ~SensorServer() = default;
};
#endif // SENSORSERVER_HPP