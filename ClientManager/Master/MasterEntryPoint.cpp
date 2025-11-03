#include "MasterEntryPoint.hpp"
#include "MasterServer.hpp"

MasterEntryPoint::MasterEntryPoint(MasterServer* master, Socket* socket,
    const std::string& ip, int port, void (MasterServer::*handler)(int, Socket*))
    : master(master), socket(socket), ip(ip), port(port), handler(handler) {
}

int MasterEntryPoint::run() {
  std::cout << "Starting listener on port " << port << std::endl;
  master->listenForever(ip, port, socket, handler);
  return EXIT_SUCCESS;
}