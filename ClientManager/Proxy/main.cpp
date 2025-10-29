#include "Proxy.hpp"
#include <csignal>
#include <cstdlib>

// Variable global para manejar señales
volatile sig_atomic_t stopRequested = 0;

void signalHandler(int signal) {
  // report the recived signal and calls stop from server
  if (signal == 2) {
    std::cerr <<"  Ctrl+C  " <<std::endl;
  } else if (signal == 15) {
    std::cerr <<"  kill PID  " <<std::endl;
  }
  return Proxy::getInstance().stopProxy();
}

void printUsage(const char* programName) {
    std::cerr << "Usage: " << programName 
              << " <proxy_ip> <proxy_port> <storage_ip> <storage_port>\n";
    std::cerr << "Example: " << programName 
              << " 0.0.0.0 8080 192.168.1.100 9000\n";
}

int main(int argc, char* argv[]) {
    // Verificar argumentos
    if (argc != 5) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    // Parsear argumentos
    std::string proxyIP = argv[1];
    int proxyPort = std::atoi(argv[2]);
    std::string storageIP = argv[3];
    int storagePort = std::atoi(argv[4]);

    // Validar puertos
    if (proxyPort <= 0 || proxyPort > 65535 || 
        storagePort <= 0 || storagePort > 65535) {
        std::cerr << "ERROR: Invalid port number (must be 1-65535)\n";
        return EXIT_FAILURE;
    }

    // Configurar manejadores de señales
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    std::cout << "=== Arduino Proxy Server ===\n";
    std::cout << "Proxy listening on: " << proxyIP << ":" << proxyPort << "\n";
    std::cout << "Storage server: " << storageIP << ":" << storagePort << "\n";
    std::cout << "Press Ctrl+C to stop...\n\n";

    try {
        // Obtener instancia del proxy y ejecutar
        Proxy& proxy = Proxy::getInstance();
        proxy.run(proxyIP, proxyPort, storageIP, storagePort);
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Proxy shutdown complete.\n";
    return EXIT_SUCCESS;
}