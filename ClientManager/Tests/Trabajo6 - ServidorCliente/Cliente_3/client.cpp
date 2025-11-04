/// @copyright Los Más Fritos - 2025

#include "../../../Util/Socket.hpp"
#include <iostream>
#include <cstring>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9090
#define PROXY_IP "127.0.0.1"
#define PROXY_PORT 9095
#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
   bool useProxy = false;
    // Determinar IP y puerto de destino
    const char* targetIP = useProxy ? PROXY_IP : SERVER_IP;
    int targetPort = useProxy ? PROXY_PORT : SERVER_PORT;
    
    std::cout << "========================================" << std::endl;
    std::cout << "        MOCK ELIMINAR LOGS DE USUARIO" << std::endl;
    std::cout << "========================================" << std::endl;
    
    Socket socket;

    if (!socket.create()) {
        std::cerr << "Error: No se pudo crear el socket" << std::endl;
        return -1;
    }

    if (!socket.connectToServer(targetIP, targetPort)) {
        std::cerr << "Error: No se pudo conectar a " << targetIP 
                  << ":" << targetPort << std::endl;
        return -1;
    }
    
    std::cout << "Conexión establecida" << std::endl;
    
    genMessage message;
    message.MID = static_cast<uint8_t>(MessageType::DELETE_USER_LOGS);
    userLogRequestCommon user;
    user.userName = "carlosBK2001.txt";
    user.id_token = 1234567;
    message.content = user;
    socket.bSendData(socket.getSocketFD(), message);
}
