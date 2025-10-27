#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>

#include "Messages.hpp"

class Socket {
protected:
    int sockfd;
    sockaddr_in address;

public:
    Socket();
    virtual ~Socket();

    // create a TCP socket using IPv4
    bool create(int domain = AF_INET, int type = SOCK_STREAM, int protocol = 0);
    bool setReuseAddr();
    bool bindSocket(const std::string& ip, int port);
    // aceppt at most 10 connections (this value will be changed in the near future)
    bool listenSocket(int backlog = 10);
    int acceptConnection();
    bool connectToServer(const std::string& ip, int port);
    ssize_t sendData(int clientSocket, const std::string& data);
    ssize_t receiveData(int clientSocket, char* buffer, size_t length);

    // using bitsery to serialize/deserialize the messages 
    ssize_t bSendData(int clientSocket, const genMessage& data);
    ssize_t bReceiveData(int clientSocket, genMessage& message);

    void closeSocket(int fd = -1);

    int getSocketFD() const;
    sockaddr_in getAddress() const;
};

#endif // SOCKET_HPP