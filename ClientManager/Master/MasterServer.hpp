/// Los más fritos 2025

#ifndef MASTERSERVER_HPP
#define MASTERSERVER_HPP

#include <iostream>
#include <cstring>
#include <string>
#include "../Util/Socket.hpp"
#include "../Util/IPConstants.hpp"

#include "MasterWorker.hpp"

class MasterEntryPoint;

class MasterServer {
 protected:
  std::string serverIP;
  
  Socket userEntryPoint;
  Socket arduinoEntryPoint;

  int userPort;
  int arduinoPort;

  std::vector<MasterEntryPoint*> listeners;

 public:
  MasterServer();
  ~MasterServer();

  int startAllListeners(const std::string& ip);

  void waitForAllListeners();

  int openConnectionEntries(std::string ip, int port, Socket* socket);
  int listenForConnections(std::string ip, int port, Socket* socket);

  // Enviar el metodo del trabajo como parametro
  int acceptConnectionRequest(Socket* socket, void (MasterServer::*handler)(int, Socket*));
  void acceptAllConnections(Socket* socket, void (MasterServer::*handler)(int, Socket*));
  void listenForever(std::string ip, int port, Socket* socket, void (MasterServer::*handler)(int, Socket*));

  void handleUserConnection(int client, Socket* socket);
  void handleAuthConnection(int client, Socket* socket);
  void handleArduinoConnection(int client, Socket* socket);
  void handleStorageConnection(int client, Socket* socket);
};

#endif