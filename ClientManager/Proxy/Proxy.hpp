#ifndef PROXY_HPP
#define PROXY_HPP

#include <string>
#include <iostream>
#include <algorithm>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include "../Util/Socket.hpp"
#include "../Util/Messages.hpp"
#include "../Util/Queue.hpp"
#include "../Util/Util.hpp"
#include "../Util/ServerDiscoveryPoint.hpp"
#include "../Util/ServerDiscover.hpp"
#include "../Util/IPConstants.hpp"

enum{
  ULTRASONICSENSOR = 100,
  UVSENSOR = 200,
  DHTSENSOR = 300
};

struct QueuedMessage {
  int arduinoSocket;
  genMessage message;
};

class Proxy {
 private:
  std::string masterServerIP;
  int masterServerPort;
  
  std::string proxyIP;
  int proxyPort;
  Socket listeningSocket;
  std::atomic<bool> running;
  
  Queue<QueuedMessage> messageQueue;
  
  std::thread storageHandlerThread;

  ServerDiscoveryPoint* discoveryPoint;
  
  std::vector<std::thread> clientThreads;
  std::mutex threadsMutex;
  
  int startProxy();
  int listenForConnections(std::string ip, int port);
  void acceptAllConnections();
  void forwardSensorData(genMessage& sensorData);
  void processMessageQueue();
  genMessage parseArduinoData(const std::string& rawData, const std::string& originIP);
    
 public:
  Proxy();
  ~Proxy();
  
  static Proxy& getInstance();
  
  void run();
  void stopProxy();
  void handleArduinoConnection(int arduinoSocket);
  int arduinoType(std::string arduinoInformatuion);

};

#endif //PROXY_HPP