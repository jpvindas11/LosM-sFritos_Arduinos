/// Los más fritos 2025
/// Ejemplo de uso del MasterServer

#include "MasterServer.hpp"
#include <signal.h>

MasterServer* globalServer = nullptr;

void signalHandler(int signal) {
  if (signal == SIGINT) {
    std::cout << "\nShutting down server..." << std::endl;
    // Aquí podrías cerrar los sockets si implementas un método shutdown()
    exit(0);
  }
}

int main(int argc, char* argv[]) {
  // Configurar el handler de señales para Ctrl+C
  signal(SIGINT, signalHandler);

  std::string ip = "127.0.0.1";  // localhost por defecto
  
  if (argc > 1) {
    ip = argv[1];
  }

  std::cout << "=== Master Server ===" << std::endl;
  std::cout << "Starting on IP: " << ip << std::endl;

  MasterServer server;
  globalServer = &server;

  // Iniciar todos los listeners en hilos separados
  if (server.startAllListeners(ip) != EXIT_SUCCESS) {
    std::cerr << "Failed to start listeners" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "\nServer running. Press Ctrl+C to stop.\n" << std::endl;
  std::cout << "Listening on:" << std::endl;
  std::cout << "  - Users:   port " << PORT_MASTER_USERS << std::endl;
  std::cout << "  - Auth:    port " << PORT_MASTER_AUTH << std::endl;
  std::cout << "  - Arduino: port " << PORT_MASTER_ARDUINO << std::endl;
  std::cout << "  - Storage: port " << PORT_MASTER_STORAGE << std::endl;

  // Esperar a que todos los hilos terminen
  // (en la práctica, esto se ejecutará indefinidamente hasta Ctrl+C)
  server.waitForAllListeners();

  return EXIT_SUCCESS;
}