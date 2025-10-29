#ifndef ARDUINO_SIMULATOR_HPP
#define ARDUINO_SIMULATOR_HPP

#include <string>
#include <thread>
#include <atomic>
#include <random>
#include <iostream>
#include <chrono>
#include "../Util/Socket.hpp"
#include "../Util/Messages.hpp"

class ArduinoSimulator {
private:
    uint16_t arduinoId;
    std::string proxyIP;
    int proxyPort;
    std::string sensorType;
    int sendIntervalMs;
    
    std::atomic<bool> running;
    std::thread senderThread;
    
    // Random number generators
    std::mt19937 rng;
    std::uniform_real_distribution<float> tempDist;
    std::uniform_real_distribution<float> humidityDist;
    std::uniform_real_distribution<float> pressureDist;
    
    // Private methods
    void senderLoop();
    genMessage generateSensorMessage();
    std::string generateSensorValue();
    sensorFileName getCurrentFileName();

public:
    ArduinoSimulator(uint16_t id, const std::string& proxyIP, int proxyPort,
                     const std::string& sensorType, int intervalMs);
    ~ArduinoSimulator();
    
    void startSending();
    void stopSending();
    void sendSensorData();
};

#endif // ARDUINO_SIMULATOR_HPP
