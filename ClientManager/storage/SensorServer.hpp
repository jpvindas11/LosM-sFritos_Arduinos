#ifndef SENSORSERVER_HPP
#define SENSORSERVER_HPP

#include <sstream>
#include <cstdio>
#include <thread>
#include <mutex>
#include <atomic>

#include "genServer/StorageServer.hpp"

#define SENSOR_FORGET_TIME 120 // 2 minutos

class SensorServer : public StorageServer {
 private:
  std::atomic<bool> running;
  std::mutex storageMutex; // Para acceso thread-safe al filesystem
  std::vector<sensorRecentData> recentData;

  void processClientInThread(int clientSocket);

 public:
  static SensorServer& getInstance();
  void run(std::string serverIP, int listeningPort);
  void stopServer();
  void handleClientConnection(int  clientSocket) override;
  void serveClient(int clientSocket, genMessage& clientRequest);
  void addToSensorLog(senAddLog& messageContent);
  std::string getSensorFileName(sensorFileName& name);
  void closeListeningSocket();
  std::string getFromBuffer(char* buffer, uint32_t size);

 private:
  int startServer(std::string serverIP, int listeningPort);
  SensorServer();
  ~SensorServer() {
  }
  /// Envía la cantidad de archivos en el sistema
  void sendFileNumber(int clientSocket, GenNumReq& messageContent);
  /// Envía los nombres de los archivos en sistema
  void sendFileNames(int clientSocket, GenNumReq& messageContent);
  /// Envía metadatos del sensor solicitados
  void sendSensorFileMetadata(int clientSocket, genSenFileReq& messageContent);
  /// Envía el tamaño del bloque solicitado
  void sendFileBlockNumber(int clientSocket, genSenFileReq& messageContent);
  /// Envía los datos del archivo en bloques
  void sendFileBlock(int clientSocket, genSenFileReq& messageContent);
  /// Agrega un sensor
  void addToSensorServer(addSensor& messageContent);
  /// Elimina un sensor
  void deleteFromSensorServer(deleteSensor& messageContent);
  /// Modifica los metadatos de un sensor
  void modifySensor(modifySensorInfp& messageContent);

  void sendRecentData(int clientSocket, GenNumReq& messageContent);


 void updateRecentSensorData(const std::string& sensorIP, const std::string& sensorType, const std::string& data);
};
#endif // SENSORSERVER_HPP
