#include "Socket.hpp"

Socket::Socket() {
    // no socket created yet
    sockfd = -1;
    // set the adress structure whit all its values in 0
    memset(&address, 0, sizeof(address));
}

Socket::~Socket() {
    // close the socket that listen for connections
    closeSocket();
}

bool Socket::create(int domain, int type, int protocol) {
    // get the socket file descriptor
    sockfd = socket(domain, type, protocol);
    // if -1 the socket was not created
    return sockfd >= 0;
}

bool Socket::setReuseAddr() {
    // reuse the given port if the server is restarted
    int opt = 1;
    // if setsockopt return -1 then the operation could not be done
    return setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) >= 0;
}

bool Socket::bindSocket(const std::string& ip, int port) {
    // IPv4 family
    address.sin_family = AF_INET;
    // set the port in its network format
    address.sin_port = htons(port);
    // set the ip in its binary value
    if (inet_pton(AF_INET, ip.c_str(), &address.sin_addr) <= 0) {
        return false;
    }
    // bind the class socket to the port to listen for connections
    return bind(sockfd, (struct sockaddr*)&address, sizeof(address)) >= 0;
}

bool Socket::listenSocket(int backlog) {
    // listen for connections using the main socket
    // the waiting queue has a backlog size
    return listen(sockfd, backlog) >= 0;
}

int Socket::acceptConnection() {
    // accept a client connection
    // nullptr because the client address is not required yet
    // return the new client file descriptor
    return accept(sockfd, nullptr, nullptr);
}

bool Socket::connectToServer(const std::string& ip, int port) {
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &address.sin_addr) <= 0) {
        return false;
    }
    // connect to server. Returns true if the connection was succesfull
    return connect(sockfd, (struct sockaddr*)&address, sizeof(address)) >= 0;
}

ssize_t Socket::sendData(int clientSocket, const std::string& data) {
    // send data to client. Returns the amount of sent bytes
    return send(clientSocket, data.c_str(), data.length(), 0);
}

ssize_t Socket::receiveData(int clientSocket, char* buffer, size_t length) {
    // read the data sent by a client. Return the amount of red bytes
    return read(clientSocket, buffer, length);
}

void Socket::closeSocket(int fd) {
    // close the listening socket if no 
    // specific socket file descriptor was provided
    if (fd == -1) {
        fd = sockfd;
    }
    // else close the client socket
    if (fd >= 0) {
        close(fd)
    };
}

int Socket::getSocketFD() const {
    return sockfd;
}

sockaddr_in Socket::getAddress() const {
    return address;
}