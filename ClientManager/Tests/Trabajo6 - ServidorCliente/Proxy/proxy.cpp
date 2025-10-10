/// @copyright Los Más Fritos - 2025

#include "../../../Util/Socket.hpp"
#include "../../../Datagrams/datagramEncoder.hpp"
#include <iostream>
#include <cstring>

#define PROXY_IP "0.0.0.0"
#define PROXY_PORT 9095
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9090
#define BUFFER_SIZE 1024

void forwardMessages(int clientSocket, Socket& proxySocket) {
    Socket clientSock;
    
    std::cout << "\n[PROXY] Conectando al servidor real " << SERVER_IP << ":" << SERVER_PORT << "..." << std::endl;
    
    // Servidor
    if (!proxySocket.connectToServer(SERVER_IP, SERVER_PORT)) {
        std::cerr << "[PROXY] Error: No se pudo conectar al servidor real" << std::endl;
        return;
    }
    
    std::cout << "[PROXY] Conectado al servidor real" << std::endl;
    
    char buffer[BUFFER_SIZE];
    
    // Recibir mensaje del cliente
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t bytesFromClient = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
    
    if (bytesFromClient > 0) {
        std::cout << "[PROXY] Recibido del cliente: " << bytesFromClient << " bytes" << std::endl;

        int messageID = getMessageID(buffer, bytesFromClient);
        std::cout << "[PROXY] Tipo de mensaje: " << messageID << std::endl;
        
        ssize_t sentToServer = send(proxySocket.getSocketFD(), buffer, bytesFromClient, 0);
        
        // Enviar al servidor
        if (sentToServer > 0) {
            std::cout << "[PROXY] Reenviado al servidor: " << sentToServer << " bytes" << std::endl;
            
            // Recibir respuesta del servidor
            memset(buffer, 0, BUFFER_SIZE);
            ssize_t bytesFromServer = recv(proxySocket.getSocketFD(), buffer, BUFFER_SIZE - 1, 0);
            
            if (bytesFromServer > 0) {
                std::cout << "[PROXY] Recibido del servidor: " << bytesFromServer << " bytes" << std::endl;
                
                // Reenviar respuesta al cliente
                ssize_t sentToClient = send(clientSocket, buffer, bytesFromServer, 0);
                
                if (sentToClient > 0) {
                    std::cout << "[PROXY] Respuesta reenviada al cliente: " 
                              << sentToClient << " bytes" << std::endl;
                } else {
                    std::cerr << "[PROXY] Error al enviar respuesta al cliente" 
                              << std::endl;
                }
            } else {
                std::cerr << "[PROXY] Error al recibir respuesta del servidor" 
                          << std::endl;
            }
        } else {
            std::cerr << "[PROXY] Error al enviar al servidor" << std::endl;
        }
    } else if (bytesFromClient == 0) {
        std::cout << "[PROXY] Cliente cerró la conexión" << std::endl;
    } else {
        std::cerr << "[PROXY] Error al recibir datos del cliente" << std::endl;
    }

    proxySocket.closeSocket();
}

int main() {
    Socket proxyListener;
    if (!proxyListener.create()) {
        std::cerr << "[PROXY] Error: No se pudo crear el socket" << std::endl;
        return -1;
    }

    if (!proxyListener.setReuseAddr()) {
        std::cerr << "[PROXY] Error: No se pudo configurar SO_REUSEADDR" 
                  << std::endl;
        return -1;
    }

    if (!proxyListener.bindSocket(PROXY_IP, PROXY_PORT)) {
        std::cerr << "[PROXY] Error: No se pudo enlazar el socket al puerto " 
                  << PROXY_PORT << std::endl;
        return -1;
    }

    if (!proxyListener.listenSocket()) {
        std::cerr << "[PROXY] Error: No se pudo poner el socket en modo escucha" 
                  << std::endl;
        return -1;
    }

    std::cout << "========================================" << std::endl;
    std::cout << "      PROXY SERVER INICIADO" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Escuchando en: " << PROXY_IP << ":" << PROXY_PORT << std::endl;
    std::cout << "Servidor real: " << SERVER_IP << ":" << SERVER_PORT << std::endl;
    std::cout << "========================================\n" << std::endl;

    while (true) {
        std::cout << "[PROXY] Esperando conexión de cliente..." << std::endl;
        
        int clientSocket = proxyListener.acceptConnection();
        if (clientSocket < 0) {
            std::cerr << "[PROXY] Error: No se pudo aceptar la conexión" << std::endl;
            continue;
        }
        std::cout << "[PROXY] Cliente conectado (fd: " << clientSocket << ")" << std::endl;
        
        Socket serverConnection;
        if (!serverConnection.create()) {
            std::cerr << "[PROXY] Error: No se pudo crear socket para servidor" 
                      << std::endl;
            proxyListener.closeSocket(clientSocket);
            continue;
        }
        forwardMessages(clientSocket, serverConnection);
        proxyListener.closeSocket(clientSocket);
        std::cout << "[PROXY] Conexión con cliente cerrada\n" << std::endl;
    }
}
