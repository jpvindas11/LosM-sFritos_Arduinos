#ifndef SENSORSERVER_HPP
#define SENSORSERVER_HPP

#include <sstream>
#include <cstdio>
#include <thread>
#include <mutex>
#include <atomic>

#include "StorageServer.hpp"


class SensorServer : public StorageServer {
 private:
  std::atomic<bool> running;
  std::mutex storageMutex; // Para acceso thread-safe al filesystem

  void processClientInThread(int clientSocket);

 public:
  static SensorServer& getInstance();
  void run(std::string serverIP, int listeningPort, 
                                          std::string masterIP, int materPort);
  void stopServer();
  void handleClientConnection(int  clientSocket) override;
  void serveClient(int clientSocket, genMessage& clientRequest);
  void addToSensorLog(senAddLog& messageContent);
  std::string getSensorFileName(sensorFileName& name);
  void closeListeningSocket();
  std::string getFromBuffer(char* buffer, uint32_t size);

 private:
  int startServer(std::string serverIP, int listeningPort
                , std::string masterIP, int materPort);
  SensorServer();
  ~SensorServer() = default;
  /// Envía la cantidad de archivos en el sistema
  void sendFileNumber(int clientSocket, GenNumReq messageContent);
  /// Envía los nombres de los archivos en sistema
  void sendFileNames(int clientSocket, GenNumReq messageContent);
  /// Envía metadatos del sensor solicitados
  void sendSensorFileMetadata(int clientSocket, genSenFileReq messageContent);
  /// Envía el tamaño del bloque solicitado
  void sendFileBlockNumber(int clientSocket, genSenFileReq messageContent);
  /// Envía los datos del archivo en bloques
  void sendFileBlock(int clientSocket, genSenFileReq messageContent);
};
#endif // SENSORSERVER_HPP
