#include "UDPSocket.hpp"
#include "Socket.hpp"
#include <vector>
#include <chrono>
#include <thread>
#include <set>

class ServerDiscover {
private:
    UDPSocket udpSocket;
    int discoveryPort;
    
    // Redes adicionales para búsqueda dirigida
    std::vector<std::string> additionalNetworks;
    
public:
    struct DiscoveredServer {
        std::string name;
        std::string ip;
        uint16_t port;
        uint8_t serverType;
        uint8_t raidMode;
    };
    
    ServerDiscover(int udp_port) 
        : discoveryPort(udp_port) {
        // Por defecto, agregar la red Ethernet del lab
        additionalNetworks.push_back("10.1.35");
    }
    
    // Permitir agregar redes adicionales manualmente
    void addNetwork(const std::string& networkPrefix) {
        additionalNetworks.push_back(networkPrefix);
    }
    
    void clearAdditionalNetworks() {
        additionalNetworks.clear();
    }
    
    std::vector<DiscoveredServer> discoverServers(int timeoutSeconds = 3) {
        std::vector<DiscoveredServer> servers;
        
        // Crear y configurar socket UDP
        if (!udpSocket.create()) {
            std::cerr << "ERROR: No se pudo crear socket UDP" << std::endl;
            return servers;
        }
        
        // Habilitar broadcast
        if (!udpSocket.setBroadcast(true)) {
            std::cerr << "ERROR: No se pudo habilitar broadcast" << std::endl;
            return servers;
        }
        
        // Configurar timeout para recepción
        udpSocket.setReceiveTimeout(0, 200000); // 200ms
        
        // Bind a cualquier puerto
        if (!udpSocket.bindSocket("0.0.0.0", 0)) {
            std::cerr << "ERROR: No se pudo hacer bind" << std::endl;
            return servers;
        }
        
        // Crear mensaje de petición
        genMessage request;
        request.MID = static_cast<uint8_t>(MessageType::SERVER_DISCOVER_REQ);
        
        serverDiscoverReq reqData;
        reqData.clientName = "Cliente1";
        request.content = reqData;
        
        std::cout << "Buscando servidores en la red local..." << std::endl;
        
        // Enviar a redes adicionales
        for (const auto& networkPrefix : additionalNetworks) {
            std::cout << "Buscando en red adicional: " << networkPrefix << ".x" << std::endl;
            sendToNetwork(networkPrefix, request);
        }

        // Enviar broadcast a la red local
        ssize_t sent = udpSocket.sendBroadcast(discoveryPort, request);
        if (sent <= 0) {
            std::cerr << "ERROR: No se pudo enviar broadcast" << std::endl;
        } else {
            std::cout << "Broadcast enviado a red local" << std::endl;
        }
        
        std::cout << "Peticiones enviadas, esperando respuestas..." << std::endl;
        
        // Usar un set para evitar servidores duplicados
        std::set<std::string> discoveredIPs;
        
        // Escuchar respuestas
        auto startTime = std::chrono::steady_clock::now();
        int consecutiveTimeouts = 0;
        const int maxConsecutiveTimeouts = 3; // Más tiempo para redes adicionales
        
        while (true) {
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                currentTime - startTime).count();
            
            // Timeout máximo de seguridad
            if (elapsed >= timeoutSeconds) {
                std::cout << "Timeout máximo alcanzado" << std::endl;
                break;
            }
            
            genMessage response;
            std::string senderIP;
            int senderPort;
            
            ssize_t bytesRead = udpSocket.receiveFrom(response, senderIP, senderPort);
            
            if (bytesRead > 0) {
                consecutiveTimeouts = 0;
                
                // Verificar si es una respuesta de descubrimiento
                if (response.MID == static_cast<uint8_t>(MessageType::SERVER_DISCOVER_RES)) {
                    try {
                        serverDiscoverRes resData = getMessageContent<serverDiscoverRes>(response);
                        
                        // Determinar la IP del servidor
                        std::string serverIP = resData.serverIP.empty() ? senderIP : resData.serverIP;
                        
                        // Evitar duplicados
                        if (discoveredIPs.find(serverIP) != discoveredIPs.end()) {
                            continue;
                        }
                        discoveredIPs.insert(serverIP);
                        
                        DiscoveredServer server;
                        server.name = resData.serverName;
                        server.ip = serverIP;
                        server.serverType = resData.serverType;
                        
                        server.raidMode = resData.raidMode;
                        
                        servers.push_back(server);
                        
                        std::cout << "Servidor encontrado: " << server.name 
                                << " en " << server.ip << ":" << server.port 
                                << " RAID: " << (server.raidMode == 1 ? "PRIMARY" : 
                                                server.raidMode == 2 ? "BACKUP" : "STANDALONE")
                                << std::endl;
                    } catch (const std::exception& e) {
                        std::cerr << "ERROR al procesar respuesta: " << e.what() << std::endl;
                    }
                }
            } else {
                consecutiveTimeouts++;
                
                if (consecutiveTimeouts >= maxConsecutiveTimeouts) {
                    std::cout << "No se detectaron más respuestas, finalizando búsqueda" << std::endl;
                    break;
                }
            }
        }
        
        udpSocket.closeSocket();
        std::cout << "Búsqueda completada. " << servers.size() << " servidor(es) encontrado(s)" << std::endl;
        return servers;
    }
    
private:
    // Envía peticiones dirigidas a toda una subred
    void sendToNetwork(const std::string& networkPrefix, const genMessage& request) {
        // Enviar a IPs comunes (Ponemos las de nuestras compus en la isla)
        std::vector<int> priorityIPs = {9, 10, 11, 12};
        
        for (int lastOctet : priorityIPs) {
            std::string targetIP = networkPrefix + "." + std::to_string(lastOctet);
            udpSocket.sendTo(targetIP, discoveryPort, request);
        }

    }

public:    
    // Versión que busca un servidor específico por nombre
    bool findServerByName(const std::string& serverName, DiscoveredServer& result, 
                          int timeoutSeconds = 3) {
        auto servers = discoverServers(timeoutSeconds);
        
        for (const auto& server : servers) {
            if (server.name == serverName) {
                result = server;
                return true;
            }
        }
        
        return false;
    }

    std::string lookForServer() {
        // Timeout más corto ya que solo necesitamos el primer servidor
        std::vector<DiscoveredServer> servers = this->discoverServers(2);

        if (servers.empty()) {
            std::cout << "No se encontraron servidores disponibles" << std::endl;
            return "NOSERVER";
        }

        return servers[0].ip;
    }
};