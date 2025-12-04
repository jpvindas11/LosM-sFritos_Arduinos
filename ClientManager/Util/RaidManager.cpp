#include "RaidManager.hpp"
#include "ServerDiscover.hpp"

/**
 * Inicia el RaidManager
 * Busca automáticamente una pareja y establece el modo
 */
bool RaidManager::start() {
    std::cout << "\nComenzar raid manager *** " << std::endl;
    std::cout << "Server: " << myServerName << " (" << myServerIP << ")" << std::endl;
    
    // Buscar pareja RAID
    if (!findRaidPair()) {
        std::cout << "No se encontró pareja RAID disponible" << std::endl;
        currentMode = RAIDMode::PRIMARY;
        std::cout << "Modo: primario" << std::endl;
    }
    
    running = true;
    
    // Iniciar threads según el modo
    if (currentMode == RAIDMode::PRIMARY) {
        startReplicationListener();
    } else if (currentMode == RAIDMode::BACKUP) {
        requestFullSync();
    }
    
    // Siempre iniciar heartbeat
    startHeartbeat();

    return true;
}

/**
 * Detiene el RaidManager
 */
void RaidManager::stop() {
    std::cout << "[RAID] Iniciando detención de RaidManager..." << std::endl;
    
    running = false;

    std::cout << "[RAID] Cerrando sockets..." << std::endl;
    discoverySocket.closeSocket();
    replicationSocket.closeSocket();
    
    // Esperar threads con timeout
    if (heartbeatThread && heartbeatThread->joinable()) {
        std::cout << "[RAID] Esperando thread de heartbeat..." << std::endl;
        heartbeatThread->join();
        delete heartbeatThread;
        heartbeatThread = nullptr;
    }
    
    if (replicationListenerThread && replicationListenerThread->joinable()) {
        std::cout << "[RAID] Esperando thread de replicación..." << std::endl;
        replicationListenerThread->join();
        delete replicationListenerThread;
        replicationListenerThread = nullptr;
    }
    
    std::cout << "[RAID] RaidManager detenido completamente" << std::endl;
}

/**
 * Notifica al RaidManager que se escribió un archivo
 * Si somos PRIMARY, lo replica al BACKUP
 */
void RaidManager::notifyFileWritten(const std::string& fileName) {
    if (currentMode != RAIDMode::PRIMARY || pairIP.empty()) {
        return;  // No hay nada que replicar
    }
    
    std::cout << "[RAID] Replicando archivo: " << fileName << std::endl;
    replicateFile(fileName);
}

RaidManager::RAIDMode RaidManager::getMode() const {
    return currentMode;
}

std::string RaidManager::getPairIP() const {
    return pairIP;
}


/**
 * Busca una pareja RAID disponible usando ServerDiscover
 */
bool RaidManager::findRaidPair() {
    std::cout << "[RAID] Buscando pareja RAID..." << std::endl;
    
    ServerDiscover discoverer(DISC_RAID_PAIR);
    discoverer.clearAdditionalNetworks();
    discoverer.addNetwork("10.1.35");  // Red del lab
    
    auto servers = discoverer.discoverServers(3);
    
    for (const auto& server : servers) {
        // Verificar que sea del mismo tipo y no sea yo mismo
        if (server.serverType == static_cast<uint8_t>(myServerType) && 
            server.ip != myServerIP) {
            
            // Preguntar si tiene pareja
            if (serverHasPair(server.ip)) {
                std::cout << "[RAID] Servidor " << server.name 
                            << " ya tiene pareja" << std::endl;
                continue;
            }
            
            // ¡Encontramos pareja!
            pairIP = server.ip;
            pairName = server.name;
            currentMode = RAIDMode::BACKUP;  // El que encuentra es BACKUP
            
            std::cout << "Pareja encontrada: " << pairName 
                        << " (" << pairIP << ")" << std::endl;
            std::cout << "Modo: BACKUP" << std::endl;
            return true;
        }
    }
    
    return false;
}

/**
 * Verifica si un servidor ya tiene pareja
 */
bool RaidManager::serverHasPair(const std::string& serverIP) {
    Socket checkSocket;
    if (!checkSocket.create()) return true;
    
    if (!checkSocket.connectToServer(serverIP, PORT_RAID_REPLICATION)) {
        checkSocket.closeSocket();
        return true;  // Asumir que tiene pareja si no responde
    }
    
    genMessage request;
    request.MID = static_cast<uint8_t>(MessageType::RAID_PAIR_REQUEST);
    raidPairRequest req;
    req.serverName = myServerName;
    req.serverIP = myServerIP;
    req.serverType = static_cast<uint8_t>(myServerType);
    request.content = req;
    
    checkSocket.bSendData(checkSocket.getSocketFD(), request);
    
    genMessage response;
    ssize_t received = checkSocket.bReceiveData(checkSocket.getSocketFD(), response);
    checkSocket.closeSocket();
    
    if (received <= 0) return true;
    
    try {
        raidPairResponse resp = getMessageContent<raidPairResponse>(response);
        return resp.hasPair == 1;
    } catch (...) {
        return true;
    }
}

/**
 * Inicia listener para solicitudes de replicación (modo PRIMARY)
 */
void RaidManager::startReplicationListener() {
    replicationListenerThread = new std::thread([this]() {
        std::cout << "[RAID] Iniciando listener de replicación..." << std::endl;
        
        if (!replicationSocket.create()) {
            std::cerr << "[RAID] Error al crear socket" << std::endl;
            return;
        }
        
        if (!replicationSocket.setReuseAddr()) {
            std::cerr << "[RAID] Error en setReuseAddr" << std::endl;
            return;
        }
        
        if (!replicationSocket.bindSocket(myServerIP, PORT_RAID_REPLICATION)) {
            std::cerr << "[RAID] Error en bind" << std::endl;
            return;
        }
        
        if (!replicationSocket.listenSocket()) {
            std::cerr << "[RAID] Error en listen" << std::endl;
            return;
        }

        struct timeval tv;
        tv.tv_sec = 1;   // Timeout de 1 segundo
        tv.tv_usec = 0;
        setsockopt(replicationSocket.getSocketFD(), SOL_SOCKET, SO_RCVTIMEO, 
                   &tv, sizeof(tv));
        
        std::cout << "[RAID] Escuchando en puerto " << PORT_RAID_REPLICATION << std::endl;
        
        while (running) {
            int clientSocket = replicationSocket.acceptConnection();
            
            // Si timeout o error, continuar el loop (chequea running)
            if (clientSocket < 0) {
                if (!running) break;  // Salir si running = false
                continue;  // Timeout normal, intentar de nuevo
            }
            
            std::thread([this, clientSocket]() {
                handleReplicationRequest(clientSocket);
            }).detach();
        }
        
        std::cout << "[RAID] Thread de replicación terminado" << std::endl;
    });
}

/**
 * Maneja solicitudes de replicación entrantes
 */
void RaidManager::handleReplicationRequest(int clientSocket) {
    genMessage request;
    if (replicationSocket.bReceiveData(clientSocket, request) <= 0) {
        close(clientSocket);
        return;
    }
    
    MessageType msgType = static_cast<MessageType>(request.MID);
    
    switch (msgType) {
        case MessageType::RAID_PAIR_REQUEST: {
            // Otro servidor pregunta si tenemos pareja
            raidPairResponse response;
            response.serverName = myServerName;
            response.serverIP = myServerIP;
            response.hasPair = (currentMode == RAIDMode::PRIMARY && !pairIP.empty()) ? 1 : 0;
            
            genMessage reply;
            reply.MID = static_cast<uint8_t>(MessageType::RAID_PAIR_RESPONSE);
            reply.content = response;
            replicationSocket.bSendData(clientSocket, reply);
            
            // Si no tenemos pareja, establecer conexión
            if (response.hasPair == 0) {
                try {
                    raidPairRequest req = getMessageContent<raidPairRequest>(request);
                    pairIP = req.serverIP;
                    pairName = req.serverName;
                    std::cout << "[RAID] Nueva pareja establecida: " << pairName 
                                << " (" << pairIP << ")" << std::endl;
                } catch (...) {}
            }
            break;
        }
        
        case MessageType::RAID_SYNC_START: {
            // Backup solicita sincronización completa
            std::cout << "[RAID] Iniciando sincronización completa..." << std::endl;
            sendFullSync(clientSocket);
            break;
        }
        
        case MessageType::RAID_HEARTBEAT: {
            // Responder heartbeat
            genMessage reply;
            reply.MID = static_cast<uint8_t>(MessageType::RAID_HEARTBEAT);
            raidHeartbeat hb;
            hb.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
            hb.isPrimary = (currentMode == RAIDMode::PRIMARY) ? 1 : 0;
            reply.content = hb;
            replicationSocket.bSendData(clientSocket, reply);
            break;
        }
        
        default:
            std::cerr << "[RAID] Mensaje desconocido: " 
                        << static_cast<int>(msgType) << std::endl;
            break;
    }
    
    close(clientSocket);
}

/**
 * Solicita sincronización completa al PRIMARY (modo BACKUP)
 */
void RaidManager::requestFullSync() {
    std::cout << "[RAID] Solicitando sincronización completa..." << std::endl;
    
    Socket syncSocket;
    if (!syncSocket.create() || !syncSocket.connectToServer(pairIP, PORT_RAID_REPLICATION)) {
        std::cerr << "[RAID] Error al conectar para sincronización" << std::endl;
        return;
    }
    
    genMessage request;
    request.MID = static_cast<uint8_t>(MessageType::RAID_SYNC_START);
    syncSocket.bSendData(syncSocket.getSocketFD(), request);
    
    // Recibir archivos
    uint32_t filesReceived = 0;
    
    while (true) {
        genMessage response;
        if (syncSocket.bReceiveData(syncSocket.getSocketFD(), response) <= 0) {
            break;
        }
        
        if (response.MID == static_cast<uint8_t>(MessageType::RAID_SYNC_FILE)) {
            try {
                raidSyncFile fileData = getMessageContent<raidSyncFile>(response);
                receiveAndSaveFile(fileData);
                filesReceived++;
            } catch (...) {
                std::cerr << "[RAID] Error al recibir archivo" << std::endl;
            }
        } else if (response.MID == static_cast<uint8_t>(MessageType::RAID_SYNC_COMPLETE)) {
            try {
                raidSyncComplete complete = getMessageContent<raidSyncComplete>(response);
                std::cout << "[RAID] Sincronización completa: " 
                            << complete.totalFiles << " archivos, "
                            << complete.totalBytes << " bytes" << std::endl;
            } catch (...) {}
            break;
        }
    }
    
    syncSocket.closeSocket();
    std::cout << "[RAID] Archivos recibidos: " << filesReceived << std::endl;
}

/**
 * Envía sincronización completa al BACKUP (modo PRIMARY)
 */
void RaidManager::sendFullSync(int clientSocket) {
    std::lock_guard<std::mutex> lock(syncMutex);
    
    std::vector<std::string> files = fileSystem->listFiles();
    uint32_t totalFiles = 0;
    uint64_t totalBytes = 0;
    
    std::cout << "[RAID] Enviando " << files.size() << " archivos..." << std::endl;
    
    for (const std::string& fileName : files) {
        // Leer archivo completo
        uint32_t fileSize = fileSystem->getFileSize(fileName);
        if (fileSize == 0) continue;
        
        char* buffer = new char[fileSize];
        uint32_t readSize = fileSize;
        
        if (!fileSystem->readFile(fileName, buffer, readSize)) {
            delete[] buffer;
            continue;
        }
        
        // Enviar en chunks de 1KB
        uint32_t totalChunks = (fileSize + 1023) / 1024;
        
        for (uint32_t chunk = 0; chunk < totalChunks; chunk++) {
            genMessage msg;
            msg.MID = static_cast<uint8_t>(MessageType::RAID_SYNC_FILE);
            
            raidSyncFile fileMsg;
            fileMsg.fileName = fileName;
            fileMsg.fileSize = fileSize;
            fileMsg.chunkNumber = chunk;
            fileMsg.totalChunks = totalChunks;
            
            uint32_t chunkSize = std::min<uint32_t>(1024, fileSize - (chunk * 1024));
            fileMsg.fileData = std::string(buffer + (chunk * 1024), chunkSize);
            
            msg.content = fileMsg;
            replicationSocket.bSendData(clientSocket, msg);
        }
        
        delete[] buffer;
        totalFiles++;
        totalBytes += fileSize;
        
        std::cout << "[RAID] Enviado: " << fileName << " (" << fileSize << " bytes)" << std::endl;
    }
    
    // Enviar mensaje de completado
    genMessage completeMsg;
    completeMsg.MID = static_cast<uint8_t>(MessageType::RAID_SYNC_COMPLETE);
    raidSyncComplete complete;
    complete.totalFiles = totalFiles;
    complete.totalBytes = totalBytes;
    completeMsg.content = complete;
    replicationSocket.bSendData(clientSocket, completeMsg);
    
    std::cout << "[RAID] Sincronización completa enviada" << std::endl;
}

/**
 * Recibe y guarda un archivo del PRIMARY
 */
void RaidManager::receiveAndSaveFile(const raidSyncFile& fileData) {
    static std::map<std::string, std::vector<std::string>> fileChunks;
    
    // Guardar chunk
    if (fileChunks.find(fileData.fileName) == fileChunks.end()) {
        fileChunks[fileData.fileName].resize(fileData.totalChunks);
    }
    
    fileChunks[fileData.fileName][fileData.chunkNumber] = fileData.fileData;
    
    // Verificar si tenemos todos los chunks
    bool complete = true;
    for (const auto& chunk : fileChunks[fileData.fileName]) {
        if (chunk.empty()) {
            complete = false;
            break;
        }
    }
    
    if (complete) {
        // Ensamblar archivo completo
        std::string fullData;
        for (const auto& chunk : fileChunks[fileData.fileName]) {
            fullData += chunk;
        }
        
        // Guardar en filesystem
        if (!fileSystem->fileExists(fileData.fileName)) {
            fileSystem->createFile(fileData.fileName);
        }
        fileSystem->writeFile(fileData.fileName, fullData.data(), fullData.size());
        
        std::cout << "[RAID] Archivo guardado: " << fileData.fileName 
                    << " (" << fullData.size() << " bytes)" << std::endl;
        
        // Limpiar chunks
        fileChunks.erase(fileData.fileName);
    }
}

/**
 * Replica un archivo específico al BACKUP
 */
void RaidManager::replicateFile(const std::string& fileName) {
    if (pairIP.empty()) return;
    
    Socket replicaSocket;
    if (!replicaSocket.create() || !replicaSocket.connectToServer(pairIP, PORT_RAID_REPLICATION)) {
        std::cerr << "[RAID] Error al conectar para replicación" << std::endl;
        return;
    }
    
    uint32_t fileSize = fileSystem->getFileSize(fileName);
    if (fileSize == 0) {
        replicaSocket.closeSocket();
        return;
    }
    
    char* buffer = new char[fileSize];
    uint32_t readSize = fileSize;
    
    if (!fileSystem->readFile(fileName, buffer, readSize)) {
        delete[] buffer;
        replicaSocket.closeSocket();
        return;
    }
    
    // Enviar archivo
    uint32_t totalChunks = (fileSize + 1023) / 1024;
    
    for (uint32_t chunk = 0; chunk < totalChunks; chunk++) {
        genMessage msg;
        msg.MID = static_cast<uint8_t>(MessageType::RAID_SYNC_FILE);
        
        raidSyncFile fileMsg;
        fileMsg.fileName = fileName;
        fileMsg.fileSize = fileSize;
        fileMsg.chunkNumber = chunk;
        fileMsg.totalChunks = totalChunks;
        
        uint32_t chunkSize = std::min<uint32_t>(1024, fileSize - (chunk * 1024));
        fileMsg.fileData = std::string(buffer + (chunk * 1024), chunkSize);
        
        msg.content = fileMsg;
        replicaSocket.bSendData(replicaSocket.getSocketFD(), msg);
    }
    
    delete[] buffer;
    replicaSocket.closeSocket();
}

/**
 * Thread de heartbeat
 */
void RaidManager::startHeartbeat() {
    heartbeatThread = new std::thread([this]() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            
            if (currentMode == RAIDMode::BACKUP && !pairIP.empty()) {
                if (!sendHeartbeat()) {
                    std::cout << "[RAID] Primario no responde, promocionando a Primario este servidor" << std::endl;
                    promoteToPRIMARY();
                }
            }
        }
    });
}

/**
 * Envía heartbeat al PRIMARY
 */
bool RaidManager::sendHeartbeat() {
    Socket hbSocket;
    if (!hbSocket.create() || !hbSocket.connectToServer(pairIP, PORT_RAID_REPLICATION)) {
        return false;
    }
    
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    setsockopt(hbSocket.getSocketFD(), SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(hbSocket.getSocketFD(), SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    
    genMessage request;
    request.MID = static_cast<uint8_t>(MessageType::RAID_HEARTBEAT);
    raidHeartbeat hb;
    hb.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    hb.isPrimary = 0;
    request.content = hb;
    
    hbSocket.bSendData(hbSocket.getSocketFD(), request);
    
    genMessage response;
    ssize_t received = hbSocket.bReceiveData(hbSocket.getSocketFD(), response);
    hbSocket.closeSocket();
    
    return received > 0;
}

/**
 * Promociona BACKUP a PRIMARY
 */
void RaidManager::promoteToPRIMARY() {
    std::lock_guard<std::mutex> lock(modeMutex);
    
    if (currentMode != RAIDMode::BACKUP) return;
    
    currentMode = RAIDMode::PRIMARY;
    std::string oldPair = pairIP;
    pairIP.clear();
    pairName.clear();
    
    std::cout << "[RAID] Promocionado a primario" << std::endl;
    std::cout << "[RAID] Servidor anterior: " << oldPair << " (caído)" << std::endl;
    
    // Iniciar listener de replicación
    startReplicationListener();
}
