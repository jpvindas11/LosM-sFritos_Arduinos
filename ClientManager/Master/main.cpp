/// Los más fritos 2025
/// Ejemplo de uso del MasterServer
#include "MasterServer.hpp"
#include <signal.h>
#include <cstring>

MasterServer* globalServer = nullptr;

void signalHandler(int signal) {
  if (signal == SIGINT) {
    std::cout << "\nShutting down server..." << std::endl;
    exit(0);
  }
}

void printUsage(const char* programName) {
  std::cout << "Usage: " << programName << " [OPTIONS]\n\n";
  std::cout << "Options:\n";
  std::cout << "  --local                Use localhost (127.0.0.1) for all server IPs\n";
  std::cout << "  --auth <IP>           Auth server IP address (required unless --local)\n";
  std::cout << "  --storage <IP>        Storage server IP address (required unless --local)\n";
  std::cout << "  --logs <IP>           Logs server IP address (required unless --local)\n\n";
  std::cout << "Examples:\n";
  std::cout << "  " << programName << " --local\n";
  std::cout << "  " << programName << " --auth 192.168.1.10 --storage 192.168.1.11 --logs 192.168.1.12\n";
}

int main(int argc, char* argv[]) {
  signal(SIGINT, signalHandler);

  std::string serverIP = "0.0.0.0";

  // Mostrar configuración
  std::cout << "Master" << std::endl;
  std::cout << "Server IP (listening): " << serverIP << std::endl;

  MasterServer server;
  globalServer = &server;

  if (server.startAllListeners(serverIP) != EXIT_SUCCESS) {
    std::cerr << "Failed to start listeners" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Server running. Press Ctrl+C to stop.\n" << std::endl;
  std::cout << "Listening on:" << std::endl;

  server.waitForAllListeners();

  return EXIT_SUCCESS;
}