/// @copyright Los Más Fritos - 2025

#include "AuthenticationServer.hpp"
#include "../../Util/IPConstants.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    AuthenticationServer server;

    if (server.initialize() != 0) {
        std::cerr << "Fallo al inicializar el servidor" << std::endl;
        return 1;
    }

    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <ip_servidor|--all|--local> <ip_logs>" << std::endl;
        std::cerr << "Ejemplo: " << argv[0] << " --all 192.168.0.10" << std::endl;
        return 1;
    }

    std::string ip;
    std::string logsIP;
    int port = PORT_MASTER_AUTH;

    // --- IP del servidor ---
    std::string arg1 = argv[1];
    if (arg1 == "--all") {
        ip = "0.0.0.0";
    } else if (arg1 == "--local") {
        ip = "127.0.0.1";
    } else {
        ip = arg1;
    }

    // --- IP de logs ---
    logsIP = argv[2];

    server.addUser("Admin", "password", USER_OWNER, USER_OWNER);
    server.setLogIP(logsIP);

    std::cout << "\n& Iniciando servidor en " << ip << ":" << port << "..." << std::endl;
    std::cout << "& IP de logs: " << logsIP << " &" << std::endl;

    if (server.listenForConnections(ip, port) != 0) {
        std::cerr << "Error al abrir el socket de escucha" << std::endl;
        return 1;
    }

    server.acceptAllConnections();
    return 0;
}
