#include "ssh.hpp"
#include "FileManager.hpp"
#include <iostream>

int main() {
    FileManager fileManager;
    std::vector<std::string> lines = fileManager.readFile("Proxy.txt");
    
    // Verificar que hay suficientes datos para al menos un servidor
    if (lines.size() < 4) {
        std::cerr << "Error: archivo Proxy.txt no tiene suficientes datos." << std::endl;
        return 1;
    }
    
    // Calcular número de servidores (cada servidor usa 4 líneas: host, user, password, command)
    int numServers = lines.size() / 4;
    
    std::cout << "Iniciando " << numServers << " servidor(es)..." << std::endl;
    
    // Procesar cada servidor
    for (int i = 0; i < numServers; i++) {
        int offset = i * 4;
        std::string host = lines[offset];
        std::string user = lines[offset + 1];
        std::string password = lines[offset + 2];
        std::string command = lines[offset + 3];
        
        std::cout << "\n--- Servidor " << (i + 1) << " ---" << std::endl;
        std::cout << "Host: " << host << std::endl;
        std::cout << "User: " << user << std::endl;
        
        SSHClient client;
        int port = 22;
        
        if (!client.connect(host, port, user)) {
            std::cerr << "Error conectando al servidor " << (i + 1) << std::endl;
            continue;
        }
        
        if (!client.authenticate(password)) {
            std::cerr << "Error autenticando en servidor " << (i + 1) << std::endl;
            client.disconnect();
            continue;
        }
        
        ssh_channel channel = client.createChannel();
        if (channel == nullptr) {
            std::cerr << "Error creando canal SSH en servidor " << (i + 1) << std::endl;
            client.disconnect();
            continue;
        }
        
        if (!client.executeCommand(channel, command, true)) {
            std::cerr << "Error ejecutando comando en servidor " << (i + 1) << std::endl;
            client.disconnect();
            continue;
        }
        
        std::cout << "Servidor " << (i + 1) << " iniciado exitosamente." << std::endl;
        client.disconnect();
    }
    
    std::cout << "\nTodos los servidores han sido procesados." << std::endl;
    return 0;
}