/// Los más fritos 2025

#ifndef MASTERSERVER_HPP
#define MASTERSERVER_HPP

#include <iostream>
#include <cstring>
#include <string>
#include "../Util/Socket.hpp"
#include "../Util/UDPSocket.hpp"
#include "../Util/IPConstants.hpp"
#include "../Util/ServerDiscover.hpp"
#include "../Util/ServerDiscoveryPoint.hpp"

#include "MasterWorker.hpp"

class MasterEntryPoint;

class MasterServer {
 protected:
  std::string serverIP;

  std::string authServerIP;
  std::string storageServerIP;
  std::string logsServerIP;

  Socket userEntryPoint;
  Socket arduinoEntryPoint;
  UDPSocket discoveryPoint;

  int userPort;
  int arduinoPort;

  std::vector<MasterEntryPoint*> listeners;

  ServerDiscoveryPoint* discoveryPoint;

  void handleMID(int client, Socket* socket);
  void handleServerStatusRequest(int client, genMessage& request);
  uint8_t checkServerConnection(const std::string& ip, int port);

 public:
  MasterServer();
  ~MasterServer();

  int startAllListeners(const std::string& ip);
  void setDestinationIPs(std::string auth, std::string storage, std::string logs);

  void waitForAllListeners();

  int openConnectionEntries(std::string ip, int port, Socket* socket);
  int listenForConnections(std::string ip, int port, Socket* socket);

  // Enviar el metodo del trabajo como parametro
  int acceptConnectionRequest(Socket* socket, void (MasterServer::*handler)(int, Socket*));
  void acceptAllConnections(Socket* socket, void (MasterServer::*handler)(int, Socket*));
  void listenForever(std::string ip, int port, Socket* socket, void (MasterServer::*handler)(int, Socket*));

  void handleUserConnection(int client, Socket* socket);
  void handleArduinoConnection(int client, Socket* socket);
};

#endif