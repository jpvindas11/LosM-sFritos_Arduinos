#include "Proxy.hpp"
#include "Util.hpp"
#include <stdexcept>
#include <regex>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <cstdio>

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
  
  Socket::ClientInfo clientInfo = Socket::getClientInfo(arduinoSocket);
  std::string originIP = clientInfo.ip;
  
  std::cout << "Arduino connection from IP: " << originIP << ", Port: " << clientInfo.port << std::endl;
  // Leer datos en texto plano del Arduino con buffer más seguro
  const size_t BUFFER_SIZE = 512;
  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);
  
  ssize_t bytesReceived = listeningSocket.receiveData(arduinoSocket, buffer, BUFFER_SIZE - 1);
  
  if (bytesReceived > 0) {
    // Asegurar terminación nula
    buffer[bytesReceived] = '\0';
    
    std::string rawData(buffer, bytesReceived);
    std::cout << "Received raw data from Arduino (socket: " << arduinoSocket 
              << ", bytes: " << bytesReceived << "): '" << rawData << "'" << std::endl;
    
    // Mostrar los datos en hexadecimal para debugging
    std::cout << "Raw data in hex: ";
    for (ssize_t i = 0; i < bytesReceived; ++i) {
      printf("%02X ", (unsigned char)buffer[i]);
    }
    std::cout << std::endl;
    
    // Parsear los datos del sensor y crear mensaje serializado
    genMessage sensorData = parseArduinoData(rawData, originIP);
    
    if (sensorData.MID != 0) {  // Verificar que el parsing fue exitoso
      std::cout << "Parsed sensor data (MID: " << static_cast<int>(sensorData.MID) << ")\n";
      
      // Encolar el mensaje para procesamiento
      QueuedMessage queuedMsg = {arduinoSocket, sensorData};
      messageQueue.enqueue(queuedMsg);
    } else {
      std::cout << "Could not parse Arduino data: '" << rawData << "'" << std::endl;
    }
  } else {
    std::cout << "Could not receive data from Arduino (socket: " << arduinoSocket 
              << "), bytes received: " << bytesReceived << std::endl;
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

genMessage Proxy::parseArduinoData(const std::string& rawData, const std::string& originIP) {
  genMessage message;
  
  // Inicializar mensaje con valores por defecto
  std::cout << "Parsing Arduino data: '" << rawData << "'" << std::endl;
  message.MID = static_cast<uint8_t>(MessageType::SEN_ADD_LOG);
  
  try {
    // Limpiar los datos de entrada de manera más agresiva
    std::string cleanData;
    for (char c : rawData) {
      // Solo mantener caracteres imprimibles ASCII y algunos caracteres de control específicos
      if ((c >= 32 && c <= 126) || c == ' ') {
        cleanData += c;
      }
    }
    
    // Trim espacios al inicio y final
    size_t start = cleanData.find_first_not_of(" \t");
    if (start != std::string::npos) {
      size_t end = cleanData.find_last_not_of(" \t");
      cleanData = cleanData.substr(start, end - start + 1);
    }

    std::vector<std::string> tokens = Util::split(cleanData); // Solo para debug
    
    std::cout << "Original data length: " << rawData.length() << std::endl;
    std::cout << "Cleaned data: '" << cleanData << "' (length: " << cleanData.length() << ")" << std::endl;
      
      // Crear estructura senAddLog
      senAddLog logData;
      
      // CORREGIDO: Remover ':' del tipo de sensor
      std::string sensorTypeRaw = tokens[0];
      // Quitar el ':' al final si existe
      if (!sensorTypeRaw.empty() && sensorTypeRaw.back() == ':') {
        sensorTypeRaw.pop_back();
      }
      
      logData.fileName.sensorType = sensorTypeRaw;  // Ahora guarda "Distancia" en lugar de "Distancia:"
      logData.fileName.id = this->arduinoType(tokens[0]);  // ID del sensor (usa tokens[0] con ':')
      logData.originIP = originIP;
      
      // Obtener fecha actual correctamente
      std::time_t now = std::time(nullptr);
      std::tm* localTime = std::localtime(&now);
      
      logData.fileName.year = localTime->tm_year + 1900;
      logData.fileName.month = localTime->tm_mon + 1;
      logData.fileName.day = localTime->tm_mday;
      
      // Formatear datos del sensor según su tipo
      char formattedBuffer[200];
      int written;
      
      // Determinar el formato según el tipo de sensor
      if (this->arduinoType(tokens[0]) == 1) { // Ultrasonic/Distancia
        written = snprintf(formattedBuffer, sizeof(formattedBuffer), 
                          "Distance:%dcm,Timestamp:%ld", std::stoi(tokens[1]), (long)now);
      } else if (this->arduinoType(tokens[0]) == 2) { // Humidity/Humedad
        written = snprintf(formattedBuffer, sizeof(formattedBuffer), 
                          "Humidity:%s%%,Timestamp:%ld", tokens[1].c_str(), (long)now);
      } else if (this->arduinoType(tokens[0]) == 3) { // UV
        written = snprintf(formattedBuffer, sizeof(formattedBuffer), 
                          "UV:%s,Timestamp:%ld", tokens[1].c_str(), (long)now);
      } else {
        // Formato genérico para sensores desconocidos
        written = snprintf(formattedBuffer, sizeof(formattedBuffer), 
                          "%s:%s,Timestamp:%ld", sensorTypeRaw.c_str(), tokens[1].c_str(), (long)now);
      }
      
      // Verificar que la escritura fue exitosa
      if (written < 0 || written >= (int)sizeof(formattedBuffer)) {
        std::cerr << "Error formatting sensor data" << std::endl;
        message.MID = 0;
        return message;
      }
      
      std::string formattedData(formattedBuffer);
      
      // Verificar longitud final (bitsery usa límite de 256 chars)
      if (formattedData.length() >= 255) {
        std::cerr << "Formatted data too long: " << formattedData.length() << " chars" << std::endl;
        formattedData = formattedData.substr(0, 254);
      }
      
      // Asignar datos
      logData.data = formattedData;
      
      // Asignar al mensaje
      message.content = logData;
      
      std::cout << "Parsed sensor type: " << sensorTypeRaw << " (ID: " << this->arduinoType(tokens[0]) << ")" << std::endl;
      std::cout << "Parsed value: " << tokens[1] << std::endl;
      std::cout << "Generated log data: '" << logData.data << "'" << std::endl;
      std::cout << "Final data length: " << logData.data.length() << std::endl;
      
    } catch (const std::out_of_range& e) {
    std::cerr << "Error parsing Arduino data: " << e.what() << std::endl;
    message.MID = 0; // Indica error en el parsing
  }

  return message;
}

int Proxy::arduinoType(std::string arduinoInformation) {
  if (arduinoInformation == "Distancia:") return 1;
  else if (arduinoInformation == "Humedad:") return 2;
  else if (arduinoInformation == "UV:") return 3;
  else {
    return -1; // Unknown type
  }
}