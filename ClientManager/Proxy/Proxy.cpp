#include "Proxy.hpp"
#include <stdexcept>

Proxy::Proxy() : running(false), messageQueue(100) {}

Proxy::~Proxy() {
    stopProxy();
}

Proxy& Proxy::getInstance() {
  static Proxy proxy;
  return proxy;
}

int Proxy::listenForConnections(std::string ip, int port) {
  if (!listeningSocket.create()) {
    std::cerr << "ERROR: Could not set the listening socket\n";
    return EXIT_FAILURE;
  }
  
  if (!listeningSocket.setReuseAddr()) {
    std::cerr << "ERROR: Could not reuse the listening socket\n";
    return EXIT_FAILURE;
  }
  
  if (!listeningSocket.bindSocket(ip, port)) {
    std::cerr << "ERROR: Could not bind socket to port\n";
    return EXIT_FAILURE;
  }
  
  if (!listeningSocket.listenSocket(5)) {
    std::cerr << "ERROR: Could not listen for connections\n";
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}

void Proxy::acceptAllConnections() {
  while (running.load() && listeningSocket.getSocketFD() >= 0) {
    try {
      int arduinoSocket = listeningSocket.acceptConnection();
      
      if (arduinoSocket == -1) {
        if (running.load()) {
          std::cerr << "Could not accept Arduino connection\n";
        }
        continue;
      }
      
      std::cout << "Arduino connected (socket: " << arduinoSocket << ")\n";
      
      // Crear thread para manejar esta conexión
      std::lock_guard<std::mutex> lock(threadsMutex);
      clientThreads.emplace_back(&Proxy::handleArduinoConnection, this, arduinoSocket);
      
    } catch (const std::runtime_error& error) {
      if (running.load()) {
        std::cerr << error.what() << std::endl;
      }
    }
  }
}

void Proxy::handleArduinoConnection(int arduinoSocket) {
  std::cout << "Thread started for Arduino socket: " << arduinoSocket << std::endl;
  
  // Solo leer UN mensaje por conexión (los Arduinos cierran después de enviar)
  genMessage sensorData;
  
  ssize_t bytesReceived = listeningSocket.bReceiveData(arduinoSocket, sensorData);
  
  if (bytesReceived > 0) {
    std::cout << "Received data from Arduino (socket: " << arduinoSocket 
              << ", MID: " << static_cast<int>(sensorData.MID) << ")\n";
    
    // Encolar el mensaje para procesamiento
    QueuedMessage queuedMsg = {arduinoSocket, sensorData};
    messageQueue.enqueue(queuedMsg);
  } else {
    std::cout << "Could not receive data from Arduino (socket: " << arduinoSocket << ")\n";
  }
  
  // Cerrar la conexión después de procesar el mensaje
  listeningSocket.closeSocket(arduinoSocket);
  std::cout << "Arduino disconnected (socket: " << arduinoSocket << ")\n";
}

void Proxy::processMessageQueue() {
  std::cout << "Storage handler thread started\n";
  
  while (running.load()) {
    try {
      QueuedMessage queuedMsg = messageQueue.dequeue();
      
      if (!running.load()) break;
      
      if (queuedMsg.arduinoSocket < 0) {
        continue;
      }
      
      forwardSensorData(queuedMsg.message);
      
    } catch (const std::exception& e) {
      if (running.load()) {
        std::cerr << "Error processing message queue: " << e.what() << std::endl;
      }
    }
  }
  
  std::cout << "Storage handler thread stopped\n";
}

void Proxy::forwardSensorData(genMessage& sensorData) {
  // Crear nueva conexión temporal para cada mensaje
  Socket tempStorageSocket;
  
  if (!tempStorageSocket.create()) {
    std::cerr << "ERROR: Could not create storage socket\n";
    return;
  }
  
  if (!tempStorageSocket.connectToServer(storageServerIP, storageServerPort)) {
    std::cerr << "ERROR: Could not connect to storage server\n";
    return;
  }
  
  ssize_t bytesSent = tempStorageSocket.bSendData(tempStorageSocket.getSocketFD(), sensorData);
  
  if (bytesSent > 0) {
    std::cout << "Data forwarded to storage server (MID: " << static_cast<int>(sensorData.MID) << ")\n";
  } else {
    std::cerr << "ERROR: Could not forward data to storage server\n";
  }
  
  tempStorageSocket.closeSocket();
}

int Proxy::startProxy(std::string proxyIP, int proxyPort, std::string storageIP, int storagePort) {
  this->proxyIP = proxyIP;
  this->proxyPort = proxyPort;
  this->storageServerIP = storageIP;
  this->storageServerPort = storagePort;
  
  if (listenForConnections(this->proxyIP, this->proxyPort) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }
  
  running.store(true);
  
  // Iniciar thread para procesar la cola de mensajes
  storageHandlerThread = std::thread(&Proxy::processMessageQueue, this);
  
  std::cout << "Proxy started on " << this->proxyIP 
    << ":" << this->proxyPort << std::endl;
  
  return EXIT_SUCCESS;
}

void Proxy::run(std::string proxyIP, int proxyPort, std::string storageIP, int storagePort) {
  try {
    if (startProxy(proxyIP, proxyPort, storageIP, storagePort) == EXIT_SUCCESS) {
      acceptAllConnections();
    }
  } catch (const std::runtime_error& error) {
    std::cerr << error.what() << std::endl;
  }
  stopProxy();
}

void Proxy::stopProxy() {
  std::cout << "Stopping proxy...\n";
  running.store(false);
  
  try {
    QueuedMessage dummyMsg = {-1, genMessage{}};
    messageQueue.enqueue(dummyMsg);
  } catch (...) {}
  
  if (storageHandlerThread.joinable()) {
    storageHandlerThread.join();
  }
  
  {
    std::lock_guard<std::mutex> lock(threadsMutex);
    for (auto& thread : clientThreads) {
      if (thread.joinable()) {
        thread.join();
      }
    }
    clientThreads.clear();
  }
  
  listeningSocket.closeSocket();
  std::cout << "Proxy stopped\n";
}