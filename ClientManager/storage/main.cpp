#include <iostream>
#include <csignal>
#include <cstdlib>
#include "StorageServer.hpp"

// Variable global para manejar el shutdown del servidor
volatile sig_atomic_t shutdownRequested = 0;

// Manejador de señales para SIGINT (Ctrl+C)
void signalHandler(int signal) {
    if (signal == SIGINT) {
        std::cout << "\nShutdown signal received. Closing server..." << std::endl;
        shutdownRequested = 1;
    }
}

int main(int argc, char* argv[]) {
    // Configuración por defecto
    std::string ip = "0.0.0.0";  // Escuchar en todas las interfaces
    int port = 8080;

    // Parsear argumentos de línea de comandos
    if (argc > 1) {
        ip = argv[1];
    }
    if (argc > 2) {
        port = std::atoi(argv[2]);
    }

    std::cout << "Starting Storage Server..." << std::endl;
    std::cout << "IP: " << ip << std::endl;
    std::cout << "Port: " << port << std::endl;

    // Configurar manejador de señales
    std::signal(SIGINT, signalHandler);

    try {
        StorageServer server;
        
        // Iniciar el servidor
        server.listenForever(ip, port);
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Storage Server stopped." << std::endl;
    return EXIT_SUCCESS;
}