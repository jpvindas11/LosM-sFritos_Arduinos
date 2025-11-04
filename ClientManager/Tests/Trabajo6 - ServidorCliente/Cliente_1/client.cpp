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
    std::cout << "        MOCK AÑADIR CLIENTE Y SUS LOGS" << std::endl;
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
    
    addUserLog user;
    user.userName = "carlosBK2001.txt";
    user.id_token = 1234567;
    std::string info = R"(2025/11/03 08:23 - Ingreso al sistema
                          2025/11/03 08:27 - Solicitud de consultar sensores
                          2025/11/03 08:29 - Ver información de sensor TEM_06
                          2025/11/03 08:45 - Cierra consulta de sensores
                          2025/11/03 08:50 - Cierre de sesión)";
    user.logInfo = info;

    genMessage message;
    message.MID = static_cast<uint8_t>(MessageType::ADD_USER_LOG);
    message.content = user;
    socket.bSendData(socket.getSocketFD(), message);

    if (!socket.create()) {
        std::cerr << "Error: No se pudo crear el socket" << std::endl;
        return -1;
    }

    if (!socket.connectToServer(targetIP, targetPort)) {
        std::cerr << "Error: No se pudo conectar a " << targetIP 
                  << ":" << targetPort << std::endl;
        return -1;
    }

    addUserLog user1;
    user1.userName = "mejiasMariana14.txt";
    user1.id_token = 4566;
    std::string info1 = R"(2025/11/03 08:30 - Ingreso al sistema
                          2025/11/03 08:33 - Solicitud de consulta de usuarios
                          2025/11/03 08:35 - Ver información de "carlosBK2001.txt" 
                          2025/11/03 08:38 - Solicita logs de "carlosBK2001.txt"
                          2025/11/03 09:05 - Cierra consulta de usuarios
                          2025/11/03 09:09 - Cierre de sesión
                          )";
    user1.logInfo = info1;
    genMessage message1;
    message1.MID = static_cast<uint8_t>(MessageType::ADD_USER_LOG);
    message1.content = user1;
    socket.bSendData(socket.getSocketFD(), message1);

    if (!socket.create()) {
        std::cerr << "Error: No se pudo crear el socket" << std::endl;
        return -1;
    }

    if (!socket.connectToServer(targetIP, targetPort)) {
        std::cerr << "Error: No se pudo conectar a " << targetIP 
                  << ":" << targetPort << std::endl;
        return -1;
    }


    addUserLog user2;
    user2.userName = "vilchezArmando.txt";
    user2.id_token = 45321;
    std::string info2 = R"(2025/11/03 10:00 - Ingreso al sistema
                          2025/11/03 10:02 - Solicitud para ver logs de sensores
                          2025/11/03 10:05 - Ver información de DIS_02_20251102.log
                          2025/11/03 10:15 - Cierra ver logs de sensores
                          2025/11/03 10:16 - Solictud de ver sensores
                          2025/11/03 10:16 - Selecciona sensores DIS_02
                          2025/11/03 10:18 - Cambio de estado DIS_02 a mantenimiento
                          2025/11/03 10:19 - Cierra ver sensores 
                          2025/11/03 10:21 - Cierre de sesión
                          )";
    user2.logInfo = info2;
    genMessage message2;
    message2.MID = static_cast<uint8_t>(MessageType::ADD_USER_LOG);
    message2.content = user2;
    socket.bSendData(socket.getSocketFD(), message2);
}
