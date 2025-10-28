#include "Proxy.hpp"
#include <stdexcept>

Proxy::Proxy() : storageSocketFD(-1), running(false) {}

Proxy::~Proxy() {
    stopProxy();
}

Proxy& Proxy::getInstance() {
  static Proxy proxy;
  return proxy;
}

bool Proxy::connectToStorageServer() {
  if (!storageSocket.create()) {
    std::cerr << "ERROR: Could not create storage socket\n";
    return false;
  }
  
  if (!storageSocket.connectToServer(storageServerIP, storageServerPort)) {
    std::cerr << "ERROR: Could not connect to storage server\n";
    return false;
  }
  
  storageSocketFD = storageSocket.getSocketFD();
  std::cout << "Connected to storage server at " << storageServerIP
    << ":" << storageServerPort << std::endl;
  return true;
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

int Proxy::acceptConnectionRequest() {
  int arduinoSocket = listeningSocket.acceptConnection();
  
  if (arduinoSocket == -1) {
    throw std::runtime_error("Could not accept Arduino connection");
  }
  
  std::cout << "Arduino connected\n";
  handleArduinoConnection(arduinoSocket);
  
  return arduinoSocket;
}

void Proxy::acceptAllConnections() {
  while (running && listeningSocket.getSocketFD() >= 0) {
    try {
      acceptConnectionRequest();
    } catch (const std::runtime_error& error) {
      if (running) {
        std::cerr << error.what() << std::endl;
      }
    }
  }
}

void Proxy::handleArduinoConnection(int arduinoSocket) {
  genMessage sensorData;
  
  // Recibir datos del Arduino
  ssize_t bytesReceived = listeningSocket.bReceiveData(arduinoSocket, sensorData);
  
  if (bytesReceived > 0) {
    // Retransmitir al servidor de almacenamiento
    forwardSensorData(arduinoSocket, sensorData);
  } else {
    std::cerr << "ERROR: Could not receive data from Arduino\n";
  }
  
  listeningSocket.closeSocket(arduinoSocket);
  std::cout << "Arduino disconnected\n";
}

void Proxy::forwardSensorData(int arduinoSocket, genMessage& sensorData) {
  // Reenviar el mensaje al servidor de almacenamiento
  ssize_t bytesSent = storageSocket.bSendData(storageSocketFD, sensorData);
  
  if (bytesSent > 0) {
    std::cout << "Data forwarded to storage server (MID: " << static_cast<int>(sensorData.MID) << ")\n";
    
    // Opcionalmente recibir confirmación del servidor
    genMessage response;
    storageSocket.bReceiveData(storageSocketFD, response);
    
    // Reenviar respuesta al Arduino si es necesario
    listeningSocket.bSendData(arduinoSocket, response);
  } else {
    std::cerr << "ERROR: Could not forward data to storage server\n";
  }
}

int Proxy::startProxy(std::string proxyIP, int proxyPort, std::string storageIP, int storagePort) {
  this->proxyIP = proxyIP;
  this->proxyPort = proxyPort;
  this->storageServerIP = storageIP;
  this->storageServerPort = storagePort;
  
  if (!connectToStorageServer()) {
    return EXIT_FAILURE;
  }
  
  if (listenForConnections(this->proxyIP, this->proxyPort) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }
  
  running = true;
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
    running = false;
    storageSocket.closeSocket();
    listeningSocket.closeSocket();
    std::cout << "Proxy stopped\n";
}