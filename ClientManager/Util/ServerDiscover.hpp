#include "UDPSocket.hpp"
#include "Socket.hpp"
#include <vector>
#include <chrono>
#include <thread>

class ServerDiscover {
private:
    UDPSocket udpSocket;
    int discoveryPort;
    
public:
    struct DiscoveredServer {
        std::string name;
        std::string ip;
        uint16_t port;
        uint8_t serverType;
    };
    
    ServerDiscover(int udp_port) 
        : discoveryPort(udp_port) {}
    
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
        
        // Configurar timeout para recepción (más corto para ser más responsivo)
        udpSocket.setReceiveTimeout(0, 200000); // 200ms en lugar de 500ms
        
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
        
        // Enviar broadcast
        ssize_t sent = udpSocket.sendBroadcast(discoveryPort, request);
        if (sent <= 0) {
            std::cerr << "ERROR: No se pudo enviar broadcast" << std::endl;
            return servers;
        }
        
        std::cout << "Broadcast enviado, esperando respuestas..." << std::endl;
        
        // Escuchar respuestas con early exit
        auto startTime = std::chrono::steady_clock::now();
        int consecutiveTimeouts = 0;
        const int maxConsecutiveTimeouts = 2; // Salir después de 2 timeouts consecutivos
        
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
                // Resetear contador de timeouts al recibir algo
                consecutiveTimeouts = 0;
                
                // Verificar si es una respuesta de descubrimiento
                if (response.MID == static_cast<uint8_t>(MessageType::SERVER_DISCOVER_RES)) {
                    try {
                        serverDiscoverRes resData = getMessageContent<serverDiscoverRes>(response);
                        
                        DiscoveredServer server;
                        server.name = resData.serverName;
                        
                        // Si el servidor no envió su IP, usar la IP del remitente
                        if (resData.serverIP.empty()) {
                            server.ip = senderIP;
                        } else {
                            server.ip = resData.serverIP;
                        }

                        server.serverType = resData.serverType;
                        
                        servers.push_back(server);
                        
                        std::cout << "Servidor encontrado: " << server.name 
                                  << " en " << server.ip << ":" << server.port 
                                  << " (desde " << senderIP << ")" << std::endl;
                    } catch (const std::exception& e) {
                        std::cerr << "ERROR al procesar respuesta: " << e.what() << std::endl;
                    }
                }
            } else {
                // Timeout o error
                consecutiveTimeouts++;
                
                // Early exit: si tenemos suficientes timeouts consecutivos, asumimos que no hay más servidores
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
        // Para esta función, usamos un timeout más corto ya que solo necesitamos el primer servidor
        std::vector<DiscoveredServer> servers = this->discoverServers(2);

        if (servers.empty()) {
            std::cout << "No se encontraron servidores disponibles" << std::endl;
            return "NOSERVER";
        }

        return servers[0].ip;
    }
};