#include "UDPSocket.hpp"

UDPSocket::UDPSocket() {
    sockfd = -1;
    memset(&address, 0, sizeof(address));
}

UDPSocket::~UDPSocket() {
    closeSocket();
}

bool UDPSocket::create(int domain) {
    // Crear socket UDP (SOCK_DGRAM)
    sockfd = socket(domain, SOCK_DGRAM, 0);
    return sockfd >= 0;
}

bool UDPSocket::setBroadcast(bool enable) {
    int opt = enable ? 1 : 0;
    return setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)) >= 0;
}

bool UDPSocket::setReuseAddr() {
    int opt = 1;
    return setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) >= 0;
}

bool UDPSocket::bindSocket(const std::string& ip, int port) {
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    
    if (ip == "0.0.0.0" || ip.empty()) {
        address.sin_addr.s_addr = INADDR_ANY;
    } else {
        if (inet_pton(AF_INET, ip.c_str(), &address.sin_addr) <= 0) {
            return false;
        }
    }
    
    return bind(sockfd, (struct sockaddr*)&address, sizeof(address)) >= 0;
}

ssize_t UDPSocket::sendTo(const std::string& destIP, int destPort, const genMessage& data) {
    // Serializar el mensaje con bitsery
    std::vector<uint8_t> buffer;
    buffer.reserve(1024);
    
    bitsery::Serializer<bitsery::OutputBufferAdapter<std::vector<uint8_t>>> ser(buffer);
    ser.object(data);
    ser.adapter().flush();
    
    // Configurar dirección de destino
    sockaddr_in destAddr;
    memset(&destAddr, 0, sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(destPort);
    
    if (inet_pton(AF_INET, destIP.c_str(), &destAddr.sin_addr) <= 0) {
        std::cerr << "ERROR: Dirección IP inválida: " << destIP << std::endl;
        return -1;
    }
    
    // Enviar el mensaje
    ssize_t sentBytes = sendto(sockfd, buffer.data(), buffer.size(), 0,
                               (struct sockaddr*)&destAddr, sizeof(destAddr));
    
    if (sentBytes == -1) {
        std::cerr << "ERROR: No se pudo enviar el mensaje UDP" << std::endl;
    }
    
    return sentBytes;
}

ssize_t UDPSocket::sendBroadcast(int destPort, const genMessage& data) {
    // Enviar a la dirección de broadcast 255.255.255.255
    return sendTo("255.255.255.255", destPort, data);
}

ssize_t UDPSocket::receiveFrom(genMessage& message, std::string& senderIP, int& senderPort) {
    std::vector<uint8_t> buffer(2048); // Buffer más grande para UDP
    sockaddr_in senderAddr;
    socklen_t senderAddrLen = sizeof(senderAddr);
    
    // Recibir datos
    ssize_t bytesRead = recvfrom(sockfd, buffer.data(), buffer.size(), 0,
                                 (struct sockaddr*)&senderAddr, &senderAddrLen);
    
    if (bytesRead <= 0) {
        if (bytesRead == 0) {
            std::cerr << "INFO: Conexión cerrada" << std::endl;
        } else {
            // Puede ser timeout u otro error
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                std::cerr << "ERROR: No se pudo recibir el mensaje UDP" << std::endl;
            }
        }
        return bytesRead;
    }
    
    // Extraer IP y puerto del remitente
    char ipBuffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &senderAddr.sin_addr, ipBuffer, INET_ADDRSTRLEN);
    senderIP = std::string(ipBuffer);
    senderPort = ntohs(senderAddr.sin_port);
    
    // Deserializar el mensaje
    buffer.resize(bytesRead);
    bitsery::Deserializer<bitsery::InputBufferAdapter<std::vector<uint8_t>>> 
        des(buffer.begin(), buffer.end());
    des.object(message);
    
    return bytesRead;
}

bool UDPSocket::setReceiveTimeout(int seconds, int microseconds) {
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = microseconds;
    
    return setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) >= 0;
}

void UDPSocket::closeSocket() {
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1;
    }
}

int UDPSocket::getSocketFD() const {
    return sockfd;
}