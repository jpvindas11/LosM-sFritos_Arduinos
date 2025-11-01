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

struct QueuedMessage {
  int arduinoSocket;
  genMessage message;
};

class Proxy {
 private:
  std::string storageServerIP;
  int storageServerPort;
  
  std::string proxyIP;
  int proxyPort;
  Socket listeningSocket;
  std::atomic<bool> running;
  
  Queue<QueuedMessage> messageQueue;
  
  std::thread storageHandlerThread;
  
  std::vector<std::thread> clientThreads;
  std::mutex threadsMutex;
  
  int startProxy(std::string proxyIP, int proxyPort, std::string storageIP,
    int storagePort);
  int listenForConnections(std::string ip, int port);
  void acceptAllConnections();
  void forwardSensorData(genMessage& sensorData);
  void processMessageQueue();
  genMessage parseArduinoData(const std::string& rawData);
    
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