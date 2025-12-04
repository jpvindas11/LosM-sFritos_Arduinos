#include "ArduinoSimulator.hpp"

ArduinoSimulator::ArduinoSimulator(const std::string& serverIP, int serverPort,
                                   const std::string& sensorType, uint16_t sensorID,
                                   unsigned long intervalMs, const std::string& fakeIP)
    : serverIP(serverIP),
      serverPort(serverPort),
      sensorType(sensorType),
      sensorID(sensorID),
      fakeIP(fakeIP),
      sendInterval(intervalMs),
      running(false),
      rng(std::random_device{}()) {
}

ArduinoSimulator::~ArduinoSimulator() {
    stop();
}

void ArduinoSimulator::start() {
    if (running.load()) {
        std::cout << "[" << sensorType << "_" << sensorID << "] Ya está ejecutándose" << std::endl;
        return;
    }
    
    running.store(true);
    simulationThread = std::thread(&ArduinoSimulator::simulationLoop, this);
    std::cout << "[" << sensorType << "_" << sensorID << "] Simulador iniciado";
    if (!fakeIP.empty()) {
        std::cout << " (IP falsa: " << fakeIP << ")";
    }
    std::cout << std::endl;
}

void ArduinoSimulator::stop() {
    if (!running.load()) {
        return;
    }
    
    running.store(false);
    if (simulationThread.joinable()) {
        simulationThread.join();
    }
    std::cout << "[" << sensorType << "_" << sensorID << "] Simulador detenido" << std::endl;
}

bool ArduinoSimulator::isRunning() const {
    return running.load();
}

float ArduinoSimulator::generateDistance() {
    std::uniform_int_distribution<int> dist(5, 200);
    return dist(rng);
}

float ArduinoSimulator::generateHumidity() {
    std::uniform_real_distribution<float> dist(20.0f, 90.0f);
    return dist(rng);
}

float ArduinoSimulator::generateUV() {
    std::uniform_real_distribution<float> dist(0.0f, 500.0f);
    return dist(rng);
}

sensorFileName ArduinoSimulator::getCurrentDate() {
    sensorFileName fileName;
    fileName.sensorType = sensorType;
    fileName.id = sensorID;
    
    time_t now = time(nullptr);
    tm* localTime = localtime(&now);
    
    fileName.year = localTime->tm_year + 1900;
    fileName.month = localTime->tm_mon + 1;
    fileName.day = localTime->tm_mday;
    
    return fileName;
}

bool ArduinoSimulator::sendDataToServer(const std::string& data) {
    try {
        Socket socket;
        
        if (!socket.create()) {
            std::cerr << "[" << sensorType << "_" << sensorID << "] Error creando socket" << std::endl;
            return false;
        }
        
        if (!socket.connectToServer(serverIP, serverPort)) {
            std::cerr << "[" << sensorType << "_" << sensorID << "] No se pudo conectar al servidor" << std::endl;
            return false;
        }
        
        // Construir mensaje: si hay IP falsa, enviarla primero
        std::string message;
        if (!fakeIP.empty()) {
            message = "IP:" + fakeIP + "|" + data + "\n";
        } else {
            message = data + "\n";
        }
        
        ssize_t sent = socket.sendData(socket.getSocketFD(), message);
        
        socket.closeSocket();
        
        if (sent <= 0) {
            std::cerr << "[" << sensorType << "_" << sensorID << "] Error enviando datos" << std::endl;
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[" << sensorType << "_" << sensorID << "] Excepción: " << e.what() << std::endl;
        return false;
    }
}

void ArduinoSimulator::simulationLoop() {
    std::cout << "[" << sensorType << "_" << sensorID << "] Loop de simulación iniciado" << std::endl;
    std::cout << "[" << sensorType << "_" << sensorID << "] Enviando datos cada " 
              << sendInterval / 1000 << " segundos a " << serverIP << ":" << serverPort << std::endl;
    
    while (running.load()) {
        // Generar datos según el tipo de sensor
        std::string sensorData;
        
        if (sensorType == "DIS") {
            // FORMATO EXACTO: "Distancia: 123 cm"
            int value = generateDistance();
            sensorData = "Distancia: " + std::to_string(value) + " cm";
        } 
        else if (sensorType == "HUM") {
            // FORMATO EXACTO: "Humedad: 65.2 %"
            float value = generateHumidity();
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << value;
            sensorData = "Humedad: " + oss.str() + " %";
        } 
        else if (sensorType == "UV") {
            // FORMATO EXACTO: "UV: 234.56 mW/m²"
            float value = generateUV();
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << value;
            sensorData = "UV: " + oss.str() + " mW/m²";
        }
        else {
            std::cerr << "[" << sensorType << "_" << sensorID << "] Tipo de sensor desconocido" << std::endl;
            break;
        }
        
        // Mostrar datos generados
        std::cout << "[" << sensorType << "_" << sensorID << "] Generado: " << sensorData;
        if (!fakeIP.empty()) {
            std::cout << " (desde " << fakeIP << ")";
        }
        std::cout << std::endl;
        
        // Enviar datos al servidor
        if (sendDataToServer(sensorData)) {
            std::cout << "[" << sensorType << "_" << sensorID << "] ✓ Datos enviados correctamente" << std::endl;
        } else {
            std::cout << "[" << sensorType << "_" << sensorID << "] ✗ Error al enviar datos" << std::endl;
        }
        
        // Esperar el intervalo especificado
        auto start = std::chrono::steady_clock::now();
        while (running.load()) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
            
            if (elapsed >= sendInterval) {
                break;
            }
            
            // Dormir por 100ms para no consumir CPU
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    std::cout << "[" << sensorType << "_" << sensorID << "] Loop de simulación finalizado" << std::endl;
}