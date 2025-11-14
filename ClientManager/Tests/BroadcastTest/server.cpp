#include "Socket.hpp"
#include "UDPSocket.hpp"
#include <thread>
#include <atomic>

class ServerWithDiscovery {
private:
    Socket tcpSocket;           // Socket TCP principal para conexiones
    UDPSocket udpSocket;        // Socket UDP para descubrimiento
    std::string serverName;
    std::string serverIP;
    int tcpPort;
    int udpPort;
    std::atomic<bool> running;
    std::thread discoveryThread;

public:
    ServerWithDiscovery(const std::string& name, const std::string& ip, 
                        int tcp_port, int udp_port = 14999)
        : serverName(name), serverIP(ip), tcpPort(tcp_port), 
          udpPort(udp_port), running(false) {}
    
    ~ServerWithDiscovery() {
        stop();
    }
    
    bool start() {
        // Iniciar servidor TCP normal
        if (!tcpSocket.create()) {
            std::cerr << "ERROR: No se pudo crear socket TCP" << std::endl;
            return false;
        }
        
        if (!tcpSocket.setReuseAddr()) {
            std::cerr << "WARN: No se pudo configurar REUSEADDR para TCP" << std::endl;
        }
        
        if (!tcpSocket.bindSocket(serverIP, tcpPort)) {
            std::cerr << "ERROR: No se pudo hacer bind en " << serverIP 
                      << ":" << tcpPort << std::endl;
            std::cerr << "Intenta con '0.0.0.0' para escuchar en todas las interfaces" << std::endl;
            return false;
        }
        
        if (!tcpSocket.listenSocket()) {
            std::cerr << "ERROR: No se pudo poner el socket en modo listen" << std::endl;
            return false;
        }
        
        std::cout << "Servidor TCP escuchando en " << serverIP 
                  << ":" << tcpPort << std::endl;
        
        // Iniciar servicio de descubrimiento UDP
        if (!startDiscoveryService()) {
            std::cerr << "ERROR: No se pudo iniciar el servicio de descubrimiento" 
                      << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool startDiscoveryService() {
        // Crear socket UDP
        if (!udpSocket.create()) {
            std::cerr << "ERROR: No se pudo crear socket UDP" << std::endl;
            return false;
        }
        
        // Configurar opciones
        if (!udpSocket.setReuseAddr()) {
            std::cerr << "WARN: No se pudo configurar REUSEADDR" << std::endl;
        }
        
        // Bind al puerto UDP (escuchar en todas las interfaces)
        if (!udpSocket.bindSocket("0.0.0.0", udpPort)) {
            std::cerr << "ERROR: No se pudo hacer bind en puerto UDP " 
                      << udpPort << std::endl;
            return false;
        }
        
        std::cout << "Servicio de descubrimiento UDP escuchando en puerto " 
                  << udpPort << std::endl;
        
        // Iniciar thread para escuchar peticiones de descubrimiento
        running = true;
        discoveryThread = std::thread(&ServerWithDiscovery::discoveryLoop, this);
        
        return true;
    }
    
    void discoveryLoop() {
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
                    std::cout << "Petición de descubrimiento recibida desde " 
                              << senderIP << ":" << senderPort << std::endl;
                    
                    // Responder con información del servidor
                    sendDiscoveryResponse(senderIP, senderPort);
                }
            }
        }
    }
    
    void sendDiscoveryResponse(const std::string& clientIP, int clientPort) {
        genMessage response;
        response.MID = static_cast<uint8_t>(MessageType::SERVER_DISCOVER_RES);
        
        serverDiscoverRes resData;
        resData.serverName = serverName;
        
        // Si serverIP es "0.0.0.0", obtener la IP real de la interfaz
        if (serverIP == "0.0.0.0" || serverIP.empty()) {
            // Usar la IP desde donde recibimos la petición (misma red local)
            // O puedes implementar lógica para obtener la IP de la interfaz
            resData.serverIP = ""; // El cliente deberá usar la IP desde donde recibió la respuesta
        } else {
            resData.serverIP = serverIP;
        }
        
        resData.serverTCPPort = tcpPort;
        resData.serverType = 0; // 0 = servidor de usuarios
        
        response.content = resData;
        
        // Enviar respuesta al cliente
        ssize_t sent = udpSocket.sendTo(clientIP, clientPort, response);
        if (sent > 0) {
            std::cout << "Respuesta de descubrimiento enviada a " 
                      << clientIP << ":" << clientPort << std::endl;
        } else {
            std::cerr << "ERROR: No se pudo enviar respuesta a " 
                      << clientIP << ":" << clientPort << std::endl;
        }
    }
    
    void stop() {
        running = false;
        if (discoveryThread.joinable()) {
            discoveryThread.join();
        }
        udpSocket.closeSocket();
        tcpSocket.closeSocket();
    }
    
    Socket& getTCPSocket() {
        return tcpSocket;
    }
};

// Ejemplo de uso:
int main() {
    // Usa "0.0.0.0" para escuchar en todas las interfaces
    // O usa tu IP específica si la conoces
    ServerWithDiscovery server("ServidorUsuarios", "0.0.0.0", 15000, 14999);
    
    if (!server.start()) {
        std::cerr << "No se pudo iniciar el servidor" << std::endl;
        return 1;
    }
    
    std::cout << "\n=== Servidor iniciado correctamente ===" << std::endl;
    std::cout << "Puerto TCP: 15000" << std::endl;
    std::cout << "Puerto UDP (descubrimiento): 14999" << std::endl;
    std::cout << "Presiona Enter para detener..." << std::endl;
    
    // Aquí puedes aceptar conexiones TCP normales en otro thread
    // mientras el servicio de descubrimiento corre en paralelo
    
    std::cin.get();
    
    std::cout << "Deteniendo servidor..." << std::endl;
    server.stop();
    std::cout << "Servidor detenido" << std::endl;
    
    return 0;
}
