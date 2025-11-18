#ifndef SERVERDiscoveryPOINT_HPP
#define SERVERDiscoveryPOINT_HPP

#include <iostream>
#include <string>
#include <atomic>
#include "UDPSocket.hpp"
#include "Thread.hpp"
#include "Messages.hpp"

class ServerDiscoveryPoint : public Thread {
private:
    UDPSocket udpSocket;
    std::string serverName;
    std::string serverIP;
    int udpPort;
    ServerType serverType;
    std::atomic<bool> running;

public:
    /**
     * Constructor
     * @param name Nombre del servidor
     * @param ip IP del servidor TCP (puede ser "0.0.0.0" para todas las interfaces)
     * @param udp_port Puerto UDP para descubir
     * @param type Tipo de servidor
     */
    ServerDiscoveryPoint(const std::string& name, 
                       const std::string& ip, 
                       int udp_port,
                       ServerType type)
        : serverName(name), 
          serverIP(ip), 
          udpPort(udp_port),
          serverType(type),
          running(false) {}
    
    virtual ~ServerDiscoveryPoint() {
        stopDiscovery();
    }
    
    /**
     * Inicializa el socket UDP y lo prepara para escuchar
     */
    bool initialize() {
        // Crear socket UDP
        if (!udpSocket.create()) {
            std::cerr << "[DiscoveryY] ERROR: No se pudo crear socket UDP" << std::endl;
            return false;
        }
        
        // Configurar opciones
        if (!udpSocket.setReuseAddr()) {
            std::cerr << "[DiscoveryY] WARN: No se pudo configurar REUSEADDR" << std::endl;
        }
        
        // Bind al puerto UDP (escuchar en todas las interfaces)
        if (!udpSocket.bindSocket("0.0.0.0", udpPort)) {
            std::cerr << "[DiscoveryY] ERROR: No se pudo hacer bind en puerto UDP " 
                      << udpPort << std::endl;
            return false;
        }
        
        std::cout << "[DiscoveryY] Servicio de descubrimiento escuchando en puerto " 
                  << udpPort << std::endl;
        
        return true;
    }
    
    /**
     * Detiene el servicio de descubrimiento
     */
    void stopDiscovery() {
        running = false;
        udpSocket.closeSocket();
    }

protected:
    /**
     * Thread principal que escucha peticiones de descubrimiento
     */
    int run() override {
        if (!initialize()) {
            return EXIT_FAILURE;
        }
        
        running = true;
        std::cout << "[DiscoveryY] Hilo de descubrimiento iniciado para " 
                  << serverName << std::endl;
        
        while (running) {
            genMessage request;
            std::string senderIP;
            int senderPort;
            
            // Esperar petición de descubrimiento (con timeout de 1 segundo)
            udpSocket.setReceiveTimeout(1, 0);
            ssize_t bytesRead = udpSocket.receiveFrom(request, senderIP, senderPort);
            
            if (bytesRead > 0) {
                // Verificar si es una petición de descubrimiento
                if (request.MID == static_cast<uint8_t>(MessageType::SERVER_DISCOVER_REQ)) {
                    std::cout << "[DiscoveryY] Petición recibida desde " 
                              << senderIP << ":" << senderPort << std::endl;
                    
                    // Responder con información del servidor
                    sendDiscoveryResponse(senderIP, senderPort);
                }
            }
        }
        
        std::cout << "[DiscoveryY] Hilo de descubrimiento detenido" << std::endl;
        return EXIT_SUCCESS;
    }

private:
    /**
     * Envía respuesta de descubrimiento al cliente
     */
    void sendDiscoveryResponse(const std::string& clientIP, int clientPort) {
        genMessage response;
        response.MID = static_cast<uint8_t>(MessageType::SERVER_DISCOVER_RES);
        
        serverDiscoverRes resData;
        resData.serverName = serverName;
        
        // Si serverIP es "0.0.0.0", dejar vacío para que el cliente use la IP de origen
        if (serverIP == "0.0.0.0" || serverIP.empty()) {
            resData.serverIP = "";
        } else {
            resData.serverIP = serverIP;
        }

        resData.serverType = static_cast<uint8_t>(serverType);
        
        response.content = resData;
        
        // Enviar respuesta al cliente
        ssize_t sent = udpSocket.sendTo(clientIP, clientPort, response);
        if (sent > 0) {
            std::cout << "[DiscoveryY] Respuesta enviada a " 
                      << clientIP << ":" << clientPort << std::endl;
        } else {
            std::cerr << "[DiscoveryY] ERROR: No se pudo enviar respuesta a " 
                      << clientIP << ":" << clientPort << std::endl;
        }
    }
};

#endif // SERVERDiscoveryPOINT_HPP