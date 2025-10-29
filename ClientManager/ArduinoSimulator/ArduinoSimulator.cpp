#include "ArduinoSimulator.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>

ArduinoSimulator::ArduinoSimulator(uint16_t id, const std::string& proxyIP, int proxyPort,
                                   const std::string& sensorType, int intervalMs)
    : arduinoId(id), proxyIP(proxyIP), proxyPort(proxyPort), 
      sensorType(sensorType), sendIntervalMs(intervalMs), running(false),
      rng(std::random_device{}()), tempDist(15.0f, 35.0f), 
      humidityDist(30.0f, 80.0f), pressureDist(980.0f, 1020.0f) {
}

ArduinoSimulator::~ArduinoSimulator() {
    stopSending();
}

void ArduinoSimulator::startSending() {
    if (running.load()) {
        std::cout << "Arduino " << arduinoId << " is already sending data\n";
        return;
    }
    
    running.store(true);
    senderThread = std::thread(&ArduinoSimulator::senderLoop, this);
    std::cout << "Arduino " << arduinoId << " (" << sensorType << ") started sending every " 
              << sendIntervalMs << "ms in parallel mode\n";
}

void ArduinoSimulator::stopSending() {
    if (running.load()) {
        running.store(false);
        if (senderThread.joinable()) {
            senderThread.join();
        }
        std::cout << "Arduino " << arduinoId << " stopped\n";
    }
}

void ArduinoSimulator::senderLoop() {
    while (running.load()) {
        try {
            sendSensorData();
            std::this_thread::sleep_for(std::chrono::milliseconds(sendIntervalMs));
        } catch (const std::exception& e) {
            std::cerr << "Arduino " << arduinoId << " error: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}

void ArduinoSimulator::sendSensorData() {
    Socket socket;
    
    if (!socket.create()) {
        throw std::runtime_error("Could not create socket");
    }
    
    if (!socket.connectToServer(proxyIP, proxyPort)) {
        throw std::runtime_error("Could not connect to proxy");
    }
    
    genMessage message = generateSensorMessage();
    ssize_t bytesSent = socket.bSendData(socket.getSocketFD(), message);
    
    if (bytesSent <= 0) {
        socket.closeSocket();
        throw std::runtime_error("Failed to send data");
    }
    
    std::string value = generateSensorValue();
    std::cout << "Arduino " << arduinoId << " (" << sensorType << ") sent: " << value << std::endl;
    
    socket.closeSocket();
}

genMessage ArduinoSimulator::generateSensorMessage() {
    genMessage message;
    message.MID = static_cast<uint8_t>(MessageType::SEN_ADD_LOG);
    
    senAddLog logData;
    logData.fileName = getCurrentFileName();
    logData.data = generateSensorValue();
    
    message.content = logData;
    return message;
}

std::string ArduinoSimulator::generateSensorValue() {
    std::ostringstream oss;
    
    if (sensorType == "TMP") {
        float temp = tempDist(rng);
        oss << std::fixed << std::setprecision(2) << temp << "°C";
    } else if (sensorType == "HUM") {
        float humidity = humidityDist(rng);
        oss << std::fixed << std::setprecision(1) << humidity << "%";
    } else if (sensorType == "PRS") {
        float pressure = pressureDist(rng);
        oss << std::fixed << std::setprecision(1) << pressure << "hPa";
    } else {
        float value = tempDist(rng);
        oss << std::fixed << std::setprecision(2) << value;
    }
    
    return oss.str();
}

sensorFileName ArduinoSimulator::getCurrentFileName() {
    sensorFileName filename;
    filename.sensorType = sensorType;
    filename.id = arduinoId;
    
    time_t now = time(0);
    tm* ltm = localtime(&now);
    
    filename.year = 1900 + ltm->tm_year;
    filename.month = 1 + ltm->tm_mon;
    filename.day = ltm->tm_mday;
    
    return filename;
}
