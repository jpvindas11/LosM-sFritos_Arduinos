#include "ssh.hpp"
#include "FileManager.hpp"
#include <iostream>


int main() {
    SSHClient client;
    FileManager fileManager;

    std::vector<std::string> lines = fileManager.readFile("Proxy.txt");
    
    // Ejemplo de uso
    std::string host = lines[0];
    int port = 22;
    std::string user = lines[1];
    std::string password = lines[2];
    std::string command = "cd Documentos/LosM-sFritos_Arduinos/ClientManager/Proxy/bin && ./proxy " + lines[3];
    
    if (!client.connect(host, port, user)) {
        std::cerr << "Error connecting to SSH server." << std::endl;
        return 1;
    }

    if (!client.authenticate(password)) {
        std::cerr << "Error authenticating with SSH server." << std::endl;
        return 1;
    }

    ssh_channel channel = client.createChannel();
    if (channel == nullptr) {
        std::cerr << "Error creating SSH channel." << std::endl;
        return 1;
    }
    if (!client.executeCommand(channel, command, true)) {
        std::cerr << "Error executing command on SSH server." << std::endl;
        return 1;
    }
    
    std::cout << "Server started in background successfully." << std::endl;
    
    client.disconnect();
    return 0;
}