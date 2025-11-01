#ifndef LOGSSERVER_HPP
#define LOGSSERVER_HPP

#include <sstream>
#include <cstdio>
#include <thread>
#include <mutex>
#include <atomic>

#include "../storage/genServer/StorageServer.hpp"


class LogsServer : public StorageServer {
 private:
  std::atomic<bool> running;
  std::mutex storageMutex; // Para acceso thread-safe al filesystem

  void processClientInThread(int clientSocket);

 public:
  static LogsServer& getInstance();
  void run(std::string serverIP, int listeningPort
         , std::string masterIP, int materPort);
  void stopServer();
  void handleClientConnection(int  clientSocket) override;
  void serveClient(int clientSocket, genMessage& clientRequest);
  void closeListeningSocket();

 private:
  int startServer(std::string serverIP, int listeningPort
                , std::string masterIP, int materPort);
  LogsServer();
  ~LogsServer() = default;
};
#endif // LOGSSERVER_HPP