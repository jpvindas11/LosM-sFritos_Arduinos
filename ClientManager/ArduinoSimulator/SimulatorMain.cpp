#include "ArduinoSimulator.hpp"
#include <iostream>
#include <vector>
#include <signal.h>

// Variable global para manejar señales
std::vector<ArduinoSimulator*> simulators;
bool keepRunning = true;

void signalHandler(int signum) {
    std::cout << "\n[MAIN] Señal de interrupción recibida. Deteniendo simuladores..." << std::endl;
    keepRunning = false;
    
    for (auto* sim : simulators) {
        sim->stop();
    }
}

int main() {
    // Configurar manejador de señales
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Configuración del servidor (usa 0.0.0.0 o la IP del proxy)
    std::string serverIP = "0.0.0.0";  // Cambia esto si necesitas una IP específica
    int serverPort = PORT_PROXY_LISTENER;  // Puerto donde escucha el proxy (PORT_PROXY_LISTENER)
    
    std::cout << "=== Simulador de Arduinos ===" << std::endl;
    std::cout << "Conectando a: " << serverIP << ":" << serverPort << std::endl;
    std::cout << std::endl;
    
    // Crear simuladores para cada tipo de sensor
    // Los intervalos coinciden con los del Arduino real:
    // - Distancia: 30000ms (30 segundos)
    // - Humedad: 32000ms (32 segundos)
    // - UV: 35000ms (35 segundos)
    
    ArduinoSimulator distanceSim(serverIP, serverPort, "DIS", 1, 30000);
    ArduinoSimulator humiditySim(serverIP, serverPort, "HUM", 2, 32000);
    ArduinoSimulator uvSim(serverIP, serverPort, "UV", 3, 35000);
    
    // Guardar referencias para el signal handler
    simulators.push_back(&distanceSim);
    simulators.push_back(&humiditySim);
    simulators.push_back(&uvSim);
    
    // Iniciar todos los simuladores
    std::cout << "Iniciando simuladores..." << std::endl;
    distanceSim.start();
    humiditySim.start();
    uvSim.start();
    
    std::cout << "\nSimuladores activos. Presiona Ctrl+C para detener." << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << std::endl;
    
    // Mantener el programa corriendo
    while (keepRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    // Detener simuladores (el signal handler ya los detuvo, pero por si acaso)
    std::cout << "\n[MAIN] Limpiando recursos..." << std::endl;
    distanceSim.stop();
    humiditySim.stop();
    uvSim.stop();
    
    std::cout << "[MAIN] Programa finalizado correctamente." << std::endl;
    
    return 0;
}