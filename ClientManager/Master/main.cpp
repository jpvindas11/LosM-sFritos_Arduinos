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
  std::string authServerIP;
  std::string storageServerIP;
  std::string logsServerIP;
  
  bool useLocal = false;

  // Parsear argumentos
  if (argc < 2) {
    std::cerr << "Error: Missing required arguments\n\n";
    printUsage(argv[0]);
    return EXIT_FAILURE;
  }

  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    
    if (arg == "--local") {
      useLocal = true;
    }
    else if (arg == "--auth") {
      if (i + 1 < argc) {
        authServerIP = argv[++i];
      } else {
        std::cerr << "Error: --auth requires an IP address\n\n";
        printUsage(argv[0]);
        return EXIT_FAILURE;
      }
    }
    else if (arg == "--storage") {
      if (i + 1 < argc) {
        storageServerIP = argv[++i];
      } else {
        std::cerr << "Error: --storage requires an IP address\n\n";
        printUsage(argv[0]);
        return EXIT_FAILURE;
      }
    }
    else if (arg == "--logs") {
      if (i + 1 < argc) {
        logsServerIP = argv[++i];
      } else {
        std::cerr << "Error: --logs requires an IP address\n\n";
        printUsage(argv[0]);
        return EXIT_FAILURE;
      }
    }
    else {
      std::cerr << "Error: Unknown argument '" << arg << "'\n\n";
      printUsage(argv[0]);
      return EXIT_FAILURE;
    }
  }

  if (useLocal) {
    authServerIP = "127.0.0.1";
    storageServerIP = "127.0.0.1";
    logsServerIP = "127.0.0.1";
  }

  else {
    if (authServerIP.empty() || storageServerIP.empty() || logsServerIP.empty()) {
      std::cerr << "Error: All server IPs must be specified (--auth, --storage, --logs)\n";
      std::cerr << "       Or use --local for localhost configuration\n\n";
      printUsage(argv[0]);
      return EXIT_FAILURE;
    }
  }

  // Mostrar configuración
  std::cout << "Master" << std::endl;
  std::cout << "Server IP (listening): " << serverIP << std::endl;
  std::cout << "Auth Server IP:        " << authServerIP << std::endl;
  std::cout << "Storage Server IP:     " << storageServerIP << std::endl;
  std::cout << "Logs Server IP:        " << logsServerIP << std::endl;
  std::cout << std::endl;

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