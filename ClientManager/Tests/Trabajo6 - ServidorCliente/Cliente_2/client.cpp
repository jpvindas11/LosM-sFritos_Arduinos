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
    std::cout << "        MOCK SOLICITAR LOGS DE USUARIO" << std::endl;
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
    userLogRequestCommon user;
    user.id_token = 4566;
    user.userName = "mejiasMariana14.txt";
    message.MID = static_cast<uint8_t>(MessageType::LOG_USER_REQUEST);
    message.content = user;
    socket.bSendData(socket.getSocketFD(), message);

    genMessage response;
    socket.bReceiveData(socket.getSocketFD(), response);
    userLogResp content = getMessageContent<userLogResp>(response);

    std::cout<<content.userName<<std::endl;
    std::cout<<content.id_token<<std::endl;
    std::cout<<content.firstBlock<<std::endl;

    if (!socket.create()) {
        std::cerr << "Error: No se pudo crear el socket" << std::endl;
        return -1;
    }

    if (!socket.connectToServer(targetIP, targetPort)) {
        std::cerr << "Error: No se pudo conectar a " << targetIP 
                  << ":" << targetPort << std::endl;
        return -1;
    }

    genMessage message1;
    message1.MID = static_cast<uint8_t>(MessageType::LOG_USER_REQUEST);
    userLogRequestCommon user1;
    user1.userName = "carlosBK2001.txt";
    user1.id_token = 1234567;
    message1.content = user1;
    socket.bSendData(socket.getSocketFD(), message1);

    genMessage response1;
    socket.bReceiveData(socket.getSocketFD(), response1);
    userLogResp content1 = getMessageContent<userLogResp>(response1);

    std::cout<<content1.userName<<std::endl;
    std::cout<<content1.id_token<<std::endl;
    std::cout<<content1.firstBlock<<std::endl;

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
    user2.userName = "arguedasMaria_2.txt";
    user2.id_token = 5436;
    std::string info2 = R"(2025/11/03 10:00 - Ingreso al sistema
                          2025/11/03 10:02 - Consulta de sensores
                          2025/11/03 10:05 - Ver información de VIB_02
                          2025/11/03 10:15 - Cierra consulta de sensores
                          2025/11/03 10:16 - Solictud de ver sensores
                          2025/11/03 10:21 - Cierre de sesión
                          )";
    user2.logInfo = info2;
    genMessage message2;
    message2.MID = static_cast<uint8_t>(MessageType::ADD_USER_LOG);
    message2.content = user2;
    socket.bSendData(socket.getSocketFD(), message2);
}
