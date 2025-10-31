
#ifndef MASTERENTRYPOINT_HPP
#define MASTERENTRYPOINT_HPP

#include <iostream>
#include <cstring>
#include <string>
#include "../Util/Socket.hpp"
#include "../Util/IPConstants.hpp"
#include "../Util/Thread.hpp"

class MasterServer;

class MasterEntryPoint : public Thread {
 private:
  MasterServer* master;
  Socket* socket;
  std::string ip;
  int port;

  // Funcion del EntryPoint
  void (MasterServer::*handler)(int, Socket*);

 public:
  MasterEntryPoint(MasterServer* master, Socket* socket, 
    const std::string& ip, int port, void (MasterServer::*handler)(int, Socket*));
  
 protected:
  int run() override;
};

#endif
