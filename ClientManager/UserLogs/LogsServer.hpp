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
  void run(std::string serverIP, int listeningPort);
  void stopServer();
  void handleClientConnection(int  clientSocket) override;
  void serveClient(int clientSocket, genMessage& clientRequest);
  void closeListeningSocket();

 private:
  LogsServer();
  ~LogsServer() = default;
  int startServer(std::string serverIP, int listeningPort);
  void sendUserLogs(int clientSocket, userLogRequestCommon& message);
  void addUserLogProcc(addUserLog& message);
  void deleteUser(userLogRequestCommon& message);
  void sendErrorMessage(int clientSocket, const std::string& error);
};
#endif // LOGSSERVER_HPP