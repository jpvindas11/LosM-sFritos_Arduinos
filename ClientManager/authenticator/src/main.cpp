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

    std::string ip = "127.0.0.1";
    int port = PORT_MASTER_AUTH;

    // Parseo de argumentos
    if (argc >= 2) {
        std::string arg1 = argv[1];
        if (arg1 == "--all") {
            ip = "0.0.0.0";
        } else if (arg1 == "--local") {
            ip = "127.0.0.1";
        } else {
            ip = arg1;
        }
    }

    if (argc >= 3) {
        try {
            port = std::stoi(argv[2]);
        } catch (const std::exception& e) {
            std::cerr << "Puerto inválido: " << argv[2] << std::endl;
            return 1;
        }
    }

    server.addUser("Admin", "password", USER_OWNER, USER_OWNER);

    std::cout << "\n& Iniciando servidor en " << ip << ":" << port << "... &" << std::endl;

    if (server.listenForConnections(ip, port) != 0) {
        std::cerr << "Error al abrir el socket de escucha" << std::endl;
        return 1;
    }

    server.acceptAllConnections();
    return 0;
}
