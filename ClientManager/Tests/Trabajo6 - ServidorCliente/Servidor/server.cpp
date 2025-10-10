/// @copyright Los Más Fritos - 2025

#include "../../../Util/Socket.hpp"
#include "../../../Users/src/AuthenticationServer.hpp"
#include "../../../Datagrams/datagramEncoder.hpp"
#include <iostream>
#include <cstring>

#define SERVER_IP "0.0.0.0"
#define SERVER_PORT 9090
#define BUFFER_SIZE 1024

// Quitar *
std::string removePadding(const char* str, size_t maxLen) {
    std::string result(str, maxLen);
    size_t pos = result.find('*');
    if (pos != std::string::npos) {
        result = result.substr(0, pos);
    }
    return result;
}

int main() {
    Socket socket;
    
    if (!socket.create()) {
        std::cerr << "Error: No se pudo crear el socket" << std::endl;
        return -1;
    }

    if (!socket.setReuseAddr()) {
        std::cerr << "Error: No se pudo configurar SO_REUSEADDR" << std::endl;
        return -1;
    }

    if (!socket.bindSocket(SERVER_IP, SERVER_PORT)) {
        std::cerr << "Error: No se pudo enlazar el socket al puerto " 
                  << SERVER_PORT << std::endl;
        return -1;
    }
    if (!socket.listenSocket()) {
        std::cerr << "Error: No se pudo poner el socket en modo escucha" << std::endl;
        return -1;
    }

    std::cout << "Servidor escuchando en " << SERVER_IP << ":" << SERVER_PORT << std::endl;

    AuthenticationServer authServer;

    if (authServer.initialize() == -1) {
        std::cerr << "Error: No se pudo iniciar el servidor de autenticación" << std::endl;
        return -1;
    }

    authServer.addUser("admin", "admin123", 'o', 'o');
    authServer.addUser("carlos", "carlos123", 'c', 'c');

    authServer.printUsers();

    while (true) {
        std::cout << "Esperando conexión de cliente..." << std::endl;

        // Aceptar conexión entrante
        int clientSocket = socket.acceptConnection();
        if (clientSocket < 0) {
            std::cerr << "Error: No se pudo aceptar la conexión" << std::endl;
            continue;
        }

        std::cout << "Cliente conectado (fd: " << clientSocket << ")" << std::endl;

        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);

        ssize_t bytesReceived = socket.receiveData(clientSocket, buffer, BUFFER_SIZE - 1);
        
        if (bytesReceived > 0) {
            std::cout << "Bytes recibidos: " << bytesReceived << std::endl;

            int messageID = getMessageID(buffer, bytesReceived);
            std::cout << "Message ID: " << messageID << std::endl;
            
            char responseBuffer[BUFFER_SIZE];
            memset(responseBuffer, 0, BUFFER_SIZE);
            int responseSize = 0;
            
            // Procesar MID
            if (messageID == MID_AUTH_REQUEST) {
                midAuthRequest authReq;

                if (decodeAuthRequest(buffer, bytesReceived, &authReq)) {
                    // Quitar los * que vienen en el usuario y contraseña
                    std::string username = removePadding(authReq.user, USER_NAME_SIZE);
                    std::string password = removePadding(authReq.pass, USER_PASSWORD_SIZE_MAX);
                    std::cout << "Solicitud de login:" << std::endl;
                    std::cout << "  Usuario: '" << username << "'" << std::endl;
                    std::cout << "  Password: '" << password << "'" << std::endl;
                    
                    // Ajustar con el lenguaje del servidor de autenticación
                    std::string loginMessage = "LOGIN " + username + " " + password;
                    authServer.setMessage(loginMessage);
                    authServer.processMessage();
                    std::string result = authServer.getMessage();

                    std::cout << "Resultado de autenticación: " << result << std::endl;

                    // Crear respuesta
                    if (result == "SUCCESS") {
                        midAuthSuccess successMsg;
                        memset(successMsg.message, 0, sizeof(successMsg.message));
                        strncpy(successMsg.message, "Login exitoso", sizeof(successMsg.message) - 1);
                        
                        responseSize = encodeAuthSuccess(&successMsg, responseBuffer, BUFFER_SIZE);
                        std::cout << "Respuesta: Login exitoso (MID_AUTH_SUCCESS)" << std::endl;
                    } else {
                        midErrorMessage errorMsg;
                        memset(errorMsg.message, 0, sizeof(errorMsg.message));

                        // Quitar ERROR para devolver al cliente
                        std::string errorText = result;
                        if (result.find("ERROR ") == 0) {
                            errorText = result.substr(6);
                        }

                        strncpy(errorMsg.message, errorText.c_str(), sizeof(errorMsg.message) - 1);
                        
                        responseSize = encodeErrorMessage(&errorMsg, responseBuffer, BUFFER_SIZE);
                        std::cout << "Respuesta: Error - " << errorText << " (MID_ERROR)" << std::endl;
                    }
                } else { // Error al decodificar
                    midErrorMessage errorMsg;
                    memset(errorMsg.message, 0, sizeof(errorMsg.message));
                    strncpy(errorMsg.message, "Formato invalido", sizeof(errorMsg.message) - 1);
                    responseSize = encodeErrorMessage(&errorMsg, responseBuffer, BUFFER_SIZE);
                    std::cout << "Error: Formato de mensaje inválido" << std::endl;
                }
            } else { // Mensaje no reconocido
                midErrorMessage errorMsg;
                memset(errorMsg.message, 0, sizeof(errorMsg.message));
                strncpy(errorMsg.message, "Comando no reconocido", sizeof(errorMsg.message) - 1);
                responseSize = encodeErrorMessage(&errorMsg, responseBuffer, BUFFER_SIZE);
                std::cout << "Error: Comando no reconocido (MID: " << messageID << ")" << std::endl;
            }
            // Enviar respuesta
            if (responseSize > 0) {
                ssize_t bytesSent = send(clientSocket, responseBuffer, responseSize, 0);
                if (bytesSent > 0) {
                    std::cout << "Respuesta enviada (" << bytesSent << " bytes)" << std::endl;
                } else std::cerr << "Error al enviar respuesta" << std::endl;
            }
        } else if (bytesReceived == 0) {
            std::cout << "Cliente cerró la conexión" << std::endl;
        } else std::cerr << "Error al recibir datos del cliente" << std::endl;
        
        // Cerrar socket del cliente
        socket.closeSocket(clientSocket);
        std::cout << "Conexión con cliente cerrada" << std::endl;
    }
}
