#ifndef SENSORSERVER_HPP
#define SENSORSERVER_HPP

#include <sstream>

#include "StorageServer.hpp"


class SensorServer : public StorageServer {
 private:
  /* data */
 public:
  SensorServer();
  ~SensorServer();

  void handleClientConnection(int  clientSocket) override;
  void serveClient(int clientSocket, genMessage& clientRequest);
  void addToSensorLog(senAddLog& messageContent);
  std::string getSensorFileName(sensorFileName& name);
};




#endif // SENSORSERVER_HPP