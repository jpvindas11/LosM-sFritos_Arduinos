#ifndef PROXY_HPP
#define PROXY_HPP

#include <string>
#include <iostream>
#include <algorithm>
#include "../Util/Socket.hpp"
#include "../Util/Messages.hpp"

class Proxy {
 private:
  std::string storageServerIP;
  int storageServerPort;
  Socket storageSocket;
  int storageSocketFD;
  
  std::string proxyIP;
  int proxyPort;
  Socket listeningSocket;
  bool running;
  
  int startProxy(std::string proxyIP, int proxyPort, std::string storageIP,
    int storagePort);
  int listenForConnections(std::string ip, int port);
  int acceptConnectionRequest();
  void acceptAllConnections();
  bool connectToStorageServer();
  void forwardSensorData(int arduinoSocket, genMessage& sensorData);
    
 public:
  Proxy();
  ~Proxy();
  
  static Proxy& getInstance();
  
  void run(std::string proxyIP, int proxyPort, 
           std::string storageIP, int storagePort);
  void stopProxy();
  void handleArduinoConnection(int arduinoSocket);

};

#endif //PROXY_HPP