#include "ArduinoSimulator.hpp"
#include <iostream>
#include <vector>
#include <signal.h>
#include <cstring>

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

void printUsage(const char* programName) {
    std::cout << "Uso: " << programName << " [opciones]" << std::endl;
    std::cout << "\nOpciones:" << std::endl;
    std::cout << "  -ip <dirección>    IP falsa para todos los sensores (ej: 192.168.1.100)" << std::endl;
    std::cout << "  -h, --help         Mostrar esta ayuda" << std::endl;
    std::cout << "\nEjemplos:" << std::endl;
    std::cout << "  " << programName << " -ip 192.168.1.100" << std::endl;
    std::cout << "  " << programName << "    (usa IPs por defecto diferentes para cada sensor)" << std::endl;
}

int main(int argc, char* argv[]) {
    // Configurar manejador de señales
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Procesar argumentos de línea de comandos
    std::string sharedFakeIP = "";
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-ip") == 0 && i + 1 < argc) {
            sharedFakeIP = argv[i + 1];
            i++; // Saltar el siguiente argumento (ya lo usamos)
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printUsage(argv[0]);
            return 0;
        } else {
            std::cerr << "Argumento desconocido: " << argv[i] << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    // Configuración del servidor
    std::string serverIP = "0.0.0.0";
    int serverPort = PORT_PROXY_LISTENER;
    
    std::cout << "=== Simulador de Arduinos ===" << std::endl;
    std::cout << "Conectando a: " << serverIP << ":" << serverPort << std::endl;
    std::cout << std::endl;
    
    // IPs para los sensores
    std::string fakeIP_Distance;
    std::string fakeIP_Humidity;
    std::string fakeIP_UV;
    
    if (!sharedFakeIP.empty()) {
        // Usar la misma IP para todos los sensores
        fakeIP_Distance = sharedFakeIP;
        fakeIP_Humidity = sharedFakeIP;
        fakeIP_UV = sharedFakeIP;
        std::cout << "Usando IP compartida para todos los sensores: " << sharedFakeIP << std::endl;
    } else {
        // Usar IPs diferentes por defecto
        fakeIP_Distance = "192.168.1.101";
        fakeIP_Humidity = "192.168.1.102";
        fakeIP_UV = "192.168.1.103";
        std::cout << "Usando IPs diferentes para cada sensor (por defecto)" << std::endl;
    }
    std::cout << std::endl;
    
    // Crear simuladores para cada tipo de sensor
    ArduinoSimulator distanceSim(serverIP, serverPort, "DIS", 1, 30000, fakeIP_Distance);
    ArduinoSimulator humiditySim(serverIP, serverPort, "HUM", 2, 32000, fakeIP_Humidity);
    ArduinoSimulator uvSim(serverIP, serverPort, "UV", 3, 35000, fakeIP_UV);
    
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
    std::cout << "Configuración de IPs simuladas:" << std::endl;
    std::cout << "  - Distancia (DIS_1): " << fakeIP_Distance << std::endl;
    std::cout << "  - Humedad (HUM_2):   " << fakeIP_Humidity << std::endl;
    std::cout << "  - UV (UV_3):         " << fakeIP_UV << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << std::endl;
    
    // Mantener el programa corriendo
    while (keepRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    // Detener simuladores
    std::cout << "\n[MAIN] Limpiando recursos..." << std::endl;
    distanceSim.stop();
    humiditySim.stop();
    uvSim.stop();
    
    std::cout << "[MAIN] Programa finalizado correctamente." << std::endl;
    return 0;
}