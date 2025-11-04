#include "ArduinoSimulator.hpp"
#include <iostream>
#include <vector>
#include <csignal>

// Vector global para manejar los simuladores
std::vector<ArduinoSimulator*> simulators;

// Manejador de señales para detener los simuladores limpiamente
void signalHandler(int signum) {
    std::cout << "\n\n=== Deteniendo simuladores... ===" << std::endl;
    for (auto* sim : simulators) {
        sim->stop();
    }
    exit(signum);
}

int main() {
    // Configurar manejador de señales
    signal(SIGINT, signalHandler);
    
    std::cout << "=== Simulador de Sensores Arduino ESP32 ===" << std::endl;
    std::cout << "===========================================\n" << std::endl;
    
    // Configuración del servidor (cambiar según tu configuración)
    std::string serverIP = "127.0.0.1";  // Cambia esto a la IP de tu MasterServer
    int serverPort = 13000;  // Puerto del Arduino en MasterServer (PORT_MASTER_ARDUINO)
    
    std::cout << "Servidor: " << serverIP << ":" << serverPort << "\n" << std::endl;
    
    // Crear simuladores para los 3 tipos de sensores
    // Simulador de distancia - cada 30 segundos
    auto* distSim = new ArduinoSimulator(serverIP, serverPort, "DIS", 1, 15000);
    simulators.push_back(distSim);
    
    // Simulador de humedad - cada 32 segundos
    auto* humSim = new ArduinoSimulator(serverIP, serverPort, "HUM", 1, 15500);
    simulators.push_back(humSim);
    
    // Simulador de UV - cada 35 segundos
    auto* uvSim = new ArduinoSimulator(serverIP, serverPort, "UV", 1, 17000);
    simulators.push_back(uvSim);
    
    std::cout << "=== Iniciando simuladores ===" << std::endl;
    std::cout << "Presiona Ctrl+C para detener\n" << std::endl;
    
    // Iniciar todos los simuladores
    for (auto* sim : simulators) {
        sim->start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Delay entre inicios
    }
    
    std::cout << "\n=== Todos los simuladores están activos ===" << std::endl;
    std::cout << "Envío de datos en progreso...\n" << std::endl;
    
    // Mantener el programa corriendo
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Verificar si todos los simuladores siguen corriendo
        bool allRunning = true;
        for (auto* sim : simulators) {
            if (!sim->isRunning()) {
                allRunning = false;
                break;
            }
        }
        
        if (!allRunning) {
            std::cout << "Algún simulador se detuvo inesperadamente" << std::endl;
            break;
        }
    }
    
    // Limpiar memoria
    for (auto* sim : simulators) {
        delete sim;
    }
    
    return 0;
}