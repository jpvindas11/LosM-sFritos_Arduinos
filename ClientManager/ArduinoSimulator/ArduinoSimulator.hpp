#ifndef ARDUINOSIMULATOR_HPP
#define ARDUINOSIMULATOR_HPP

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <random>
#include <atomic>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "../Util/Socket.hpp"
#include "../Util/Messages.hpp"

class ArduinoSimulator {
private:
    std::string serverIP;
    int serverPort;
    std::string sensorType;  // "DIS", "HUM", "UV"
    uint16_t sensorID;
    
    std::atomic<bool> running;
    std::thread simulationThread;
    
    // Intervalos de envío (en milisegundos)
    unsigned long sendInterval;
    
    // Generador de números aleatorios
    std::mt19937 rng;
    
    // Funciones para generar datos aleatorios
    float generateDistance();    // 5-200 cm
    float generateHumidity();     // 20-90 %
    float generateUV();          // 0-500 mW/m²
    
    // Función principal de simulación
    void simulationLoop();
    
    // Enviar datos al servidor
    bool sendDataToServer(const std::string& data);
    
    // Obtener fecha actual
    sensorFileName getCurrentDate();

public:
    ArduinoSimulator(const std::string& serverIP, int serverPort, 
                     const std::string& sensorType, uint16_t sensorID,
                     unsigned long intervalMs);
    ~ArduinoSimulator();
    
    void start();
    void stop();
    bool isRunning() const;
};

#endif // ARDUINOSIMULATOR_HPP