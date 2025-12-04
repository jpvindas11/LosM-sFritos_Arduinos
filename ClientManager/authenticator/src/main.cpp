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

    server.addUser("Admin", "passwordH0#", USER_OWNER, USER_OWNER);

    std::cout << "\n& Iniciando servidor de Usuarios" << std::endl;

    if (server.listenForConnections("0.0.0.0", PORT_MASTER_AUTH) != 0) {
        std::cerr << "Error al abrir el socket de escucha" << std::endl;
        return 1;
    }

    server.acceptAllConnections();
    return 0;
}
