#include "ArduinoSimulator.hpp"
#include <vector>
#include <memory>
#include <signal.h>
#include <thread>

std::vector<std::unique_ptr<ArduinoSimulator>> simulators;
std::atomic<bool> keepRunning(true);

void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ". Stopping simulators...\n";
    keepRunning.store(false);
    
    for (auto& simulator : simulators) {
        simulator->stopSending();
    }
}

int main(int argc, char* argv[]) {
    // Configurar manejador de señales
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Configuración por defecto
    std::string proxyIP = "127.0.0.1";
    int proxyPort = 8080;
    int numSimulators = 3;
    
    if (argc >= 3) {
        proxyIP = argv[1];
        proxyPort = std::stoi(argv[2]);
    }
    if (argc >= 4) {
        numSimulators = std::stoi(argv[3]);
    }
    
    std::cout << "Starting " << numSimulators << " Arduino simulators in PARALLEL mode\n";
    std::cout << "Proxy: " << proxyIP << ":" << proxyPort << std::endl;
    
    // Tipos de sensores disponibles
    std::vector<std::string> sensorTypes = {"TMP", "HUM", "PRS"};
    
    // Crear y arrancar simuladores en paralelo
    for (int i = 0; i < numSimulators; ++i) {
        uint16_t arduinoId = 1000 + i;
        std::string sensorType = sensorTypes[i % sensorTypes.size()];
        int interval = 2000 + (i * 300); // Intervalos ligeramente diferentes
        
        auto simulator = std::make_unique<ArduinoSimulator>(
            arduinoId, proxyIP, proxyPort, sensorType, interval
        );
        
        simulator->startSending();
        simulators.push_back(std::move(simulator));
        
        // Pequeña pausa entre inicios
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    
    std::cout << "All " << numSimulators << " simulators started in PARALLEL mode.\n";
    std::cout << "Each Arduino is sending data concurrently in its own thread.\n";
    std::cout << "Press Ctrl+C to stop...\n\n";
    
    // Mantener el programa corriendo
    while (keepRunning.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "All simulators stopped. Exiting...\n";
    return 0;
}