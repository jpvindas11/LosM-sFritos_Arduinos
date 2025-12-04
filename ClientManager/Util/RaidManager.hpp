#ifndef RAIDMANAGER_HPP
#define RAIDMANAGER_HPP

#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <map>
#include "UDPSocket.hpp"
#include "Socket.hpp"
#include "Messages.hpp"
#include "IPConstants.hpp"
#include "../FileSystem/src/FileSystem.hpp"

class ServerDiscover;

class RaidManager {
public:
    enum class RAIDMode {
        STANDALONE,    // Sin pareja
        PRIMARY,       // Acepta escrituras y replica
        BACKUP         // Recibe réplicas del primario
    };

    uint8_t getRaidModeForDiscovery() const {
        switch (currentMode) {
            case RAIDMode::STANDALONE:
                return 0;
            case RAIDMode::PRIMARY:
                return 1;
            case RAIDMode::BACKUP:
                return 2;
            default:
                return 0;
        }
    }

private:
    // Configuración
    std::string myServerName;
    std::string myServerIP;
    ServerType myServerType;
    FileSystem* fileSystem;  // Referencia al filesystem
    
    // Estado RAID
    RAIDMode currentMode;
    std::string pairIP;
    std::string pairName;
    
    // Threads
    std::thread* heartbeatThread;
    std::thread* replicationListenerThread;
    std::atomic<bool> running;
    
    // Sockets
    UDPSocket discoverySocket;
    Socket replicationSocket;
    
    // Mutex para operaciones thread-safe
    std::mutex modeMutex;
    std::mutex syncMutex;
    
public:
    RaidManager(const std::string& serverName, 
                const std::string& serverIP,
                ServerType serverType,
                FileSystem* fs)
        : myServerName(serverName),
          myServerIP(serverIP),
          myServerType(serverType),
          fileSystem(fs),
          currentMode(RAIDMode::STANDALONE),
          heartbeatThread(nullptr),
          replicationListenerThread(nullptr),
          running(false) {}
    
    ~RaidManager() {
        stop();
    }
    
    /**
     * Inicia el RaidManager
     * Busca automáticamente una pareja y establece el modo
     */
    bool start();
    
    /**
     * Detiene el RaidManager
     */
    void stop();
    
    /**
     * Notifica al RaidManager que se escribió un archivo
     * Si somos PRIMARY, lo replica al BACKUP
     */
    void notifyFileWritten(const std::string& fileName);
    
    RAIDMode getMode() const;
    
    std::string getPairIP() const;

private:
    /**
     * Busca una pareja RAID disponible usando ServerDiscover
     */
    bool findRaidPair();
    
    /**
     * Verifica si un servidor ya tiene pareja
     */
    bool serverHasPair(const std::string& serverIP);
    
    /**
     * Inicia listener para solicitudes de replicación (modo PRIMARY)
     */
    void startReplicationListener();
    
    /**
     * Maneja solicitudes de replicación entrantes
     */
    void handleReplicationRequest(int clientSocket);
    
    /**
     * Solicita sincronización completa al PRIMARY (modo BACKUP)
     */
    void requestFullSync();
    
    /**
     * Envía sincronización completa al BACKUP (modo PRIMARY)
     */
    void sendFullSync(int clientSocket);
    
    /**
     * Recibe y guarda un archivo del PRIMARY
     */
    void receiveAndSaveFile(const raidSyncFile& fileData);
    /**
     * Replica un archivo específico al BACKUP
     */
    void replicateFile(const std::string& fileName);
    
    /**
     * Thread de heartbeat
     */
    void startHeartbeat();
    
    /**
     * Envía heartbeat al PRIMARY
     */
    bool sendHeartbeat();
    
    /**
     * Promociona BACKUP a PRIMARY
     */
    void promoteToPRIMARY();
};

#endif // RAIDMANAGER_HPP