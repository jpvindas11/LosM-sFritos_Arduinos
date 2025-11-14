#ifndef UDPSOCKET_HPP
#define UDPSOCKET_HPP

#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>

#include "Messages.hpp"

class UDPSocket {
protected:
    int sockfd;
    sockaddr_in address;

public:
    UDPSocket();
    virtual ~UDPSocket();

    // Crear socket UDP
    bool create(int domain = AF_INET);
    
    // Configurar para permitir broadcast
    bool setBroadcast(bool enable = true);
    
    // Configurar para reutilizar dirección
    bool setReuseAddr();
    
    // Bind para servidor (escuchar en un puerto específico)
    bool bindSocket(const std::string& ip, int port);
    
    // Enviar datos a una dirección específica
    ssize_t sendTo(const std::string& destIP, int destPort, const genMessage& data);
    
    // Enviar broadcast a toda la red local
    ssize_t sendBroadcast(int destPort, const genMessage& data);
    
    // Recibir datos (devuelve la dirección del remitente)
    ssize_t receiveFrom(genMessage& message, std::string& senderIP, int& senderPort);
    
    // Configurar timeout para receiveFrom
    bool setReceiveTimeout(int seconds, int microseconds = 0);
    
    void closeSocket();
    
    int getSocketFD() const;

    struct ClientInfo {
        std::string ip;
        uint16_t port;
    };
};

#endif // UDPSOCKET_HPP