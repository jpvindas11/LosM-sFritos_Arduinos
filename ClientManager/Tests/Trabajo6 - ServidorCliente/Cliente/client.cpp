/// @copyright Los Más Fritos - 2025

#include "../../../Util/Socket.hpp"
#include "../../../Datagrams/datagramEncoder.hpp"
#include <iostream>
#include <cstring>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9090
#define PROXY_IP "127.0.0.1"
#define PROXY_PORT 9095
#define BUFFER_SIZE 1024

void printUsage(const char* programName) {
    std::cout << "Uso: " << programName << " [--proxy]" << std::endl;
    std::cout << "  --proxy    Conectar a través del servidor proxy" << std::endl;
    std::cout << "  (sin args) Conectar directamente al servidor" << std::endl;
}

int main(int argc, char* argv[]) {
    bool useProxy = false;
    
    // Verificar argumentos
    if (argc > 1) {
        if (strcmp(argv[1], "--proxy") == 0) {
            useProxy = true;
        } else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            printUsage(argv[0]);
            return 0;
        } else {
            std::cerr << "Argumento no reconocido: " << argv[1] << std::endl;
            printUsage(argv[0]);
            return -1;
        }
    }
    
    // Determinar IP y puerto de destino
    const char* targetIP = useProxy ? PROXY_IP : SERVER_IP;
    int targetPort = useProxy ? PROXY_PORT : SERVER_PORT;
    
    std::cout << "========================================" << std::endl;
    std::cout << "        CLIENTE DE AUTENTICACIÓN" << std::endl;
    std::cout << "========================================" << std::endl;
    if (useProxy) {
        std::cout << "Modo: A TRAVÉS DE PROXY" << std::endl;
        std::cout << "Conectando a proxy: " << targetIP << ":" << targetPort << std::endl;
    } else {
        std::cout << "Modo: CONEXIÓN DIRECTA" << std::endl;
        std::cout << "Conectando a servidor: " << targetIP << ":" << targetPort << std::endl;
    }
    std::cout << "========================================\n" << std::endl;
    
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
    
    /*
    // Pedir usuario
    std::string username, password;
    std::cout << "\n=== Sistema de Login ===" << std::endl;
    std::cout << "Usuario: ";
    std::cin >> username;
    std::cout << "Contraseña: ";
    std::cin >> password;
    
    midAuthRequest authReq;
    
    // Poner asteriscos para identificar espacios vacios
    memset(authReq.user, '*', USER_NAME_SIZE);
    memset(authReq.pass, '*', USER_PASSWORD_SIZE_MAX);

    size_t userLen = std::min(username.length(), (size_t)USER_NAME_SIZE);
    memcpy(authReq.user, username.c_str(), userLen);
    
    size_t passLen = std::min(password.length(), (size_t)USER_PASSWORD_SIZE_MAX);
    memcpy(authReq.pass, password.c_str(), passLen);
    
    // Codificar el mensaje
    char sendBuffer[BUFFER_SIZE];
    int messageSize = encodeAuthRequest(&authReq, sendBuffer, BUFFER_SIZE);
    
    if (messageSize <= 0) {
        std::cerr << "Error: No se pudo codificar el mensaje" << std::endl;
        socket.closeSocket();
        return -1;
    }
    */

    genMessage message;
    message.MID = static_cast<uint8_t>(MessageType::SEN_FILE_METD_REQ);
    auto& content = message.content.emplace<genSenFileReq>();
    content.id_token = 1234;
    
    std::cout << "\nEnviando solicitud de login..." << std::endl;
    
    // Enviar mensaje
    //ssize_t bytesSent = send(socket.getSocketFD(), sendBuffer, messageSize, 0);
    ssize_t bytesSent = socket.bSendData(socket.getSocketFD(), message);
    if (bytesSent <= 0) {
        std::cerr << "Error: No se pudo enviar el mensaje" << std::endl;
        socket.closeSocket();
        return -1;
    }
    
    std::cout << "Mensaje enviado (" << bytesSent << " bytes)" << std::endl;
    
    // Recibir respuesta
    //char recvBuffer[BUFFER_SIZE];
    //memset(recvBuffer, 0, BUFFER_SIZE);
    
    //ssize_t bytesReceived = socket.receiveData(socket.getSocketFD(), recvBuffer, BUFFER_SIZE - 1);
    
    genMessage message1;
    ssize_t bytesReceived = socket.bReceiveData(socket.getSocketFD(), message1);

    if (bytesReceived <= 0) {return EXIT_FAILURE;}

    /*if (bytesReceived > 0) {
        std::cout << "\n=== Respuesta del Servidor ===" << std::endl;
        
        int messageID = getMessageID(recvBuffer, bytesReceived);
        
        if (messageID == MID_AUTH_SUCCESS) {
            midAuthSuccess successMsg;
            if (decodeAuthSuccess(recvBuffer, bytesReceived, &successMsg)) {
                std::string message(successMsg.message);
                std::cout << "Login exitoso: " << message << std::endl;
            } else {
                std::cout << "Error al decodificar respuesta" << std::endl;

            }
        } else if (messageID == MID_ERROR) {
            midErrorMessage errorMsg;
            if (decodeErrorMessage(recvBuffer, bytesReceived, &errorMsg)) {
                std::string message(errorMsg.message);
                std::cout << "Error: " << message << std::endl;
            } else {
                std::cout << "Error al decodificar mensaje de error" << std::endl;
            }
        } else {
            std::cout << "Respuesta desconocida del servidor (MID: " << messageID << ")" << std::endl;
        }
        
    } else if (bytesReceived == 0) {
        std::cout << "Servidor cerró la conexión" << std::endl;
    } else {
        std::cerr << "Error al recibir respuesta del servidor" << std::endl;
    }*/

    senFileMetDRes newMessage = getMessageContent<senFileMetDRes>(message1);

    std::cout<<newMessage.fileName.sensorType<<std::endl;
    std::cout<<newMessage.size<<std::endl;
    std::cout<<newMessage.lastAccessTime<<std::endl;
    
    socket.closeSocket();
    std::cout << "\nConexión cerrada" << std::endl;

}
