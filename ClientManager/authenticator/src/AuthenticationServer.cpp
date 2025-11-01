/// @copyright Los Más Fritos - 2025

#include "AuthenticationServer.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <iomanip>
#include <vector>

AuthenticationServer::AuthenticationServer() : connectedUsersCount(0), counterMutex(1) {
    this->fs = new FileSystem();
    this->fs->mount("diskAuth.bin");
}

AuthenticationServer::~AuthenticationServer() {
    // El nuevo FileSystem maneja unmount automáticamente en su destructor
    delete fs;
}

int AuthenticationServer::initialize() {
    // Inicializar libsodium
    if (sodium_init() < 0) {
        std::cerr << "Error: No se pudo inicializar libsodium" << std::endl;
        return -1;
    }
    
    // Crear el archivo para almacenar usuarios si no existe
    if (!fs->fileExists("user_data.csv")) {
        if (!fs->createFile("user_data.csv")) {
            std::cerr << "Error: No se pudo crear el archivo de usuarios" << std::endl;
            return -1;
        }
        std::cout << "Archivo de usuarios creado exitosamente" << std::endl;
    } else {
        std::cout << "Archivo de usuarios ya existe, cargando usuarios..." << std::endl;
        this->loadUsers();
    }

    return 0;
}

int AuthenticationServer::openConnectionRequestSocket(std::string ip, int port) {
    if (!this->listeningSocket.create()) {
        std::cerr << "ERROR: Could not create the listening socket" << std::endl;
        return EXIT_FAILURE;
    }
    if (!this->listeningSocket.setReuseAddr()) {
        std::cerr << "ERROR: Could not set reuse address on listening socket" << std::endl;
        return EXIT_FAILURE;
    }
    if (!this->listeningSocket.bindSocket(ip, port)) {
        std::cerr << "ERROR: Could not bind socket to port " << port << std::endl;
        return EXIT_FAILURE;
    }
    
    this->serverIP = ip;
    this->listeningPort = port;
    
    return EXIT_SUCCESS;
}

int AuthenticationServer::listenForConnections(std::string ip, int port) {
    if (this->openConnectionRequestSocket(ip, port) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }
    if (!this->listeningSocket.listenSocket()) {
        std::cerr << "ERROR: Could not listen for connections" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "AuthenticationServer listening on " << ip << ":" << port << std::endl;
    return EXIT_SUCCESS;
}

int AuthenticationServer::acceptConnectionRequest() {
    int clientSocket = this->listeningSocket.acceptConnection();

    if (clientSocket == -1) {
        throw std::runtime_error("Could not accept client connection");
    }

    std::cout << "Client connected on socket " << clientSocket << std::endl;
    this->handleClientConnection(clientSocket);

    return clientSocket;
}

void AuthenticationServer::acceptAllConnections() {
    while (this->listeningSocket.getSocketFD() >= 0) {
        try {
            this->acceptConnectionRequest();
        } catch (const std::exception& e) {
            std::cerr << "Error accepting connection: " << e.what() << std::endl;
        }
    }
}

void AuthenticationServer::handleClientConnection(int clientSocket) {
    genMessage receivedMsg;

    ssize_t bytesRead = this->listeningSocket.bReceiveData(clientSocket, receivedMsg);
    
    if (bytesRead <= 0) {
        std::cerr << "Error al recibir datos o conexión cerrada" << std::endl;
        this->listeningSocket.closeSocket(clientSocket);
        return;
    }

    genMessage response;
    MessageType msgType = static_cast<MessageType>(receivedMsg.MID);
    
    try {
        switch (msgType) {
            case MessageType::AUTH_LOGIN_REQ: {
                auto req = getMessageContent<authLoginReq>(receivedMsg);
                response = processLoginRequest(req);
                break;
            }
            case MessageType::AUTH_LOGOUT: {
                auto req = getMessageContent<authLogout>(receivedMsg);
                response = processLogoutRequest(req);
                break;
            }
            case MessageType::AUTH_USER_CREATE: {
                auto req = getMessageContent<authCreateUser>(receivedMsg);
                response = processCreateUserRequest(req);
                break;
            }
            case MessageType::AUTH_USER_DELETE: {
                auto req = getMessageContent<authDeleteUser>(receivedMsg);
                response = processDeleteUserRequest(req);
                break;
            }
            case MessageType::AUTH_USER_MODIFY_PASS: {
                auto req = getMessageContent<authModifyUserPass>(receivedMsg);
                response = processModPassRequest(req);
                break;
            }
            case MessageType::AUTH_USER_MODIFY_RANK: {
                auto req = getMessageContent<authModifyUserRank>(receivedMsg);
                response = processModRankRequest(req);
                break;
            }
            default: {
                response.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
                response.content = errorCommonMsg{"Comando no reconocido"};
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error procesando mensaje: " << e.what() << std::endl;
        response.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
        response.content = errorCommonMsg{"Error interno servidor"};
    }

    ssize_t bytesSent = this->listeningSocket.bSendData(clientSocket, response);
    
    if (bytesSent <= 0) {
        std::cerr << "Error enviando respuesta al cliente" << std::endl;
    }
    
    this->listeningSocket.closeSocket(clientSocket);
}


void AuthenticationServer::listenForever(std::string ip, int port) {
    this->listenForConnections(ip, port);
    this->acceptAllConnections();
}


void AuthenticationServer::generateSalt(unsigned char* salt) {
    randombytes_buf(salt, crypto_pwhash_SALTBYTES);
}

bool AuthenticationServer::hashPassword(const std::string& password,
                                       const unsigned char* salt,
                                       unsigned char* hash) {
    if (crypto_pwhash(hash, 32, password.c_str(), password.length(), salt,
                     crypto_pwhash_OPSLIMIT_INTERACTIVE,
                     crypto_pwhash_MEMLIMIT_INTERACTIVE,
                     crypto_pwhash_ALG_DEFAULT) != 0) {
        return false;
    }
    return true;
}

bool AuthenticationServer::verifyPassword(const std::string& password,
                                         const std::string& storedHash,
                                         const std::string& storedSalt) {
    unsigned char salt[crypto_pwhash_SALTBYTES];
    if (storedSalt.length() != crypto_pwhash_SALTBYTES) {
        return false;
    }
    memcpy(salt, storedSalt.c_str(), crypto_pwhash_SALTBYTES);
    
    unsigned char hash[32];
    if (!hashPassword(password, salt, hash)) {
        return false;
    }
    
    if (storedHash.length() != 32) {
        return false;
    }
    return sodium_memcmp(hash, storedHash.c_str(), 32) == 0;
}

genMessage AuthenticationServer::processLoginRequest(const authLoginReq& req) {
    std::lock_guard<std::mutex> lock(requestMutex);
    genMessage response;
    
    auto it = users.find(req.user);
    if (it == users.end()) {
        response.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
        response.content = errorCommonMsg{"Usuario no existe"};
        return response;
    }
    
    if (it->second.isConnected) {
        response.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
        response.content = errorCommonMsg{"Usuario ya conectado"};
        return response;
    }
    
    if (!verifyPassword(req.pass, it->second.passwordHash, it->second.salt)) {
        response.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
        response.content = errorCommonMsg{"Contraseña incorrecta"};
        return response;
    }

    it->second.isConnected = true;
    counterMutex.wait();
    connectedUsersCount++;
    counterMutex.signal();
    
    // Crear token
    token userToken;
    userToken.id = connectedUsersCount;
    userToken.name = req.user;
    userToken.userType = it->second.rank;
    
    time_t now = time(nullptr);
    struct tm* tm_info = localtime(&now);
    userToken.hour = tm_info->tm_hour;
    userToken.minute = tm_info->tm_min;
    
    response.MID = static_cast<uint8_t>(MessageType::AUTH_LOGIN_SUCCESS);
    response.content = authLoginSuccess{userToken};
    
    std::cout << "Login exitoso: " << req.user << std::endl;
    return response;
}

genMessage AuthenticationServer::processLogoutRequest(const authLogout& req) {
    std::lock_guard<std::mutex> lock(requestMutex);
    genMessage response;
    
    auto it = users.find(req.user);
    if (it == users.end()) {
        response.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
        response.content = errorCommonMsg{"Usuario no existe"};
        return response;
    }
    
    if (!it->second.isConnected) {
        response.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
        response.content = errorCommonMsg{"Usuario no está conectado"};
        return response;
    }

    it->second.isConnected = false;
    counterMutex.wait();
    connectedUsersCount--;
    counterMutex.signal();

    response.MID = static_cast<uint8_t>(MessageType::OK_COMMON_MSG);
    response.content = okCommonMsg{"Logout logrado"};
    
    std::cout << "Logout exitoso: " << req.user << std::endl;
    return response;
}

genMessage AuthenticationServer::processCreateUserRequest(const authCreateUser& req) {
    std::lock_guard<std::mutex> lock(requestMutex);
    genMessage response;
    
    auto it = users.find(req.newUser);
    if (it != users.end()) {
        response.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
        response.content = errorCommonMsg{"Usuario ya existe"};
        return response;
    }

    if (this->registerUser(req.newUser, req.pass, req.rank, req.rank)) {
        response.MID = static_cast<uint8_t>(MessageType::OK_COMMON_MSG);
        response.content = okCommonMsg{"Usuario registrado"};
    } else {
        response.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
        response.content = errorCommonMsg{"No se pudo registrar al user"};
    }

    std::cout << "Usuario registrado correctamente: " << req.newUser << std::endl;

    return response;
}
genMessage AuthenticationServer::processDeleteUserRequest(const authDeleteUser& req) {
    std::lock_guard<std::mutex> lock(requestMutex);
    genMessage response;
    
    auto it = users.find(req.deleteUser);
    if (it == users.end()) {
        response.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
        response.content = errorCommonMsg{"Usuario no existe"};
        return response;
    }

    if (this->deleteUser(req.deleteUser)) {
        response.MID = static_cast<uint8_t>(MessageType::OK_COMMON_MSG);
        response.content = okCommonMsg{"Usuario eliminado"};
    } else {
        response.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
        response.content = errorCommonMsg{"No se pudo eliminar al user"};
    }

    std::cout << "Usuario eliminado correctamente: " << req.deleteUser << std::endl;

    return response;
}
genMessage AuthenticationServer::processModPassRequest(const authModifyUserPass& req) {
    std::lock_guard<std::mutex> lock(requestMutex);
    genMessage response;
    
    auto it = users.find(req.user);
    if (it == users.end()) {
        response.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
        response.content = errorCommonMsg{"Usuario no existe"};
        return response;
    }

    this->changePassword(req.user, req.newPassword);
    response.MID = static_cast<uint8_t>(MessageType::OK_COMMON_MSG);
    response.content = okCommonMsg{"Usuario modificado"};

    std::cout << "Usuario modificado (password) correctamente: " << req.user << std::endl;

    return response;
}
genMessage AuthenticationServer::processModRankRequest(const authModifyUserRank req) {
   std::lock_guard<std::mutex> lock(requestMutex);
    genMessage response;

    auto it = users.find(req.user);
    if (it == users.end()) {
        response.MID = static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG);
        response.content = errorCommonMsg{"Usuario no existe"};
        return response;
    }

    this->changePermissions(req.user, req.rank, req.rank);
    response.MID = static_cast<uint8_t>(MessageType::OK_COMMON_MSG);
    response.content = okCommonMsg{"Usuario modificado"};

    std::cout << "Usuario modificado (rank) correctamente: " << req.user << std::endl;

    return response;
}


bool AuthenticationServer::registerUser(const std::string& username,
    const std::string& password, char type, char permission) {
    std::lock_guard<std::mutex> lock(usersMutex);
    
    if (users.find(username) != users.end()) {
        std::cout << "Error: Usuario '" << username << "' ya existe" << std::endl;
        return false;
    }
    
    if (password.length() < 4) {
        std::cout << "Error: Contraseña muy corta (mínimo 4 caracteres)" << std::endl;
        return false;
    }
    
    unsigned char salt[crypto_pwhash_SALTBYTES];
    generateSalt(salt);
    
    unsigned char hash[32];
    if (!hashPassword(password, salt, hash)) {
        std::cout << "Error: No se pudo hashear la contraseña" << std::endl;
        return false;
    }
    
    // Crear estructura user_t
    user_t user;
    memset(&user, 0, sizeof(user_t));
    
    user.isUsed = '1';
    
    memset(user.name, FILLER, USER_NAME_SIZE);
    size_t nameLen = std::min(username.length(), (size_t)USER_NAME_SIZE);
    memcpy(user.name, username.c_str(), nameLen);
    
    hexLiterals(hash, 32, user.hash, USER_HASH_SIZE);
    hexLiterals(salt, crypto_pwhash_SALTBYTES, user.salt, USER_SALT_SIZE);
    
    user.type = type;
    user.permissions = permission;
    
    time_t now = time(nullptr);
    struct tm* tm_info = localtime(&now);
    
    char buf[12];
    sprintf(buf, "%02d", tm_info->tm_mday);
    memcpy(user.day, buf, 2);
    sprintf(buf, "%02d", tm_info->tm_mon + 1);
    memcpy(user.month, buf, 2);
    sprintf(buf, "%02d", tm_info->tm_hour);
    memcpy(user.hour, buf, 2);
    sprintf(buf, "%02d", tm_info->tm_min);
    memcpy(user.minute, buf, 2);
    
    user.separator = '/';
    
    if (!fs->appendFile("user_data.csv", reinterpret_cast<const char*>(&user), 
                       sizeof(user_t))) {
        std::cout << "Error: No se pudo escribir usuario en el filesystem" << std::endl;
        return false;
    }
    
    // Agregar a memoria
    AuthUser newUser;
    newUser.username = username;
    newUser.passwordHash.assign(reinterpret_cast<char*>(hash), 32);
    newUser.salt.assign(reinterpret_cast<char*>(salt), crypto_pwhash_SALTBYTES);
    newUser.isConnected = false;
    newUser.rank = type;
    
    users[username] = newUser;
    
    std::cout << "Usuario '" << username << "' registrado exitosamente" << std::endl;
    return true;
}

bool AuthenticationServer::updateUserInFile(const std::string& username,
                                           std::function<void(user_t*)> updateFn) {
    uint32_t fileSize = fs->getFileSize("user_data.csv");
    if (fileSize == 0) return false;
    
    uint32_t numUsers = fileSize / sizeof(user_t);
    
    char* buffer = new char[fileSize];
    uint32_t bytesRead = fileSize;
    
    if (!fs->readFile("user_data.csv", buffer, bytesRead)) {
        delete[] buffer;
        return false;
    }
    
    bool found = false;
    for (uint32_t i = 0; i < numUsers; i++) {
        user_t* userRecord = reinterpret_cast<user_t*>(buffer + (i * sizeof(user_t)));
        
        if (userRecord->isUsed != '1') continue;
        
        std::string recordName(userRecord->name, USER_NAME_SIZE);
        recordName.erase(std::remove(recordName.begin(), recordName.end(), FILLER),
                        recordName.end());
        
        if (recordName == username) {
            updateFn(userRecord);
            found = true;
            break;
        }
    }
    
    bool success = false;
    if (found) {
        success = fs->writeFile("user_data.csv", buffer, fileSize);
    }
    
    delete[] buffer;
    return success;
}

bool AuthenticationServer::addUser(const std::string& username,
                                   const std::string& password,
                                   char type,
                                   char permission) {
    return registerUser(username, password, type, permission);
}

std::unordered_map<std::string, AuthUser>* AuthenticationServer::getUserMap() {
    return &this->users;
}

void AuthenticationServer::hexLiterals(const unsigned char* input,
                                      size_t inputLen,
                                      char* output,
                                      size_t outputLen) {
    for (size_t i = 0; i < inputLen && (i * 2) < outputLen; i++) {
        sprintf(output + (i * 2), "%02x", input[i]);
    }
}

void AuthenticationServer::hexToBytes(const std::string& hexString,
                                     unsigned char* output,
                                     size_t outputLen) {
    for (size_t i = 0; i < outputLen && i * 2 < hexString.length(); i++) {
        std::string byteString = hexString.substr(i * 2, 2);
        output[i] = static_cast<unsigned char>(
            std::stoi(byteString, nullptr, 16));
    }
}

void AuthenticationServer::loadUsers() {
    // Limpiar usuarios en memoria
    users.clear();
    
    // Obtener tamaño del archivo
    uint32_t fileSize = fs->getFileSize("user_data.csv");
    
    if (fileSize == 0) {
        std::cout << "Archivo de usuarios vacío" << std::endl;
        return;
    }
    
    // Calcular número de usuarios
    uint32_t numUsers = fileSize / sizeof(user_t);
    
    std::cout << "Cargando " << numUsers << " usuarios..." << std::endl;
    
    // Leer todo el archivo
    char* buffer = new char[fileSize];
    uint32_t bytesRead = fileSize;
    
    if (!fs->readFile("user_data.csv", buffer, bytesRead)) {
        std::cerr << "Error al leer archivo de usuarios" << std::endl;
        delete[] buffer;
        return;
    }
    
    // Procesar cada usuario
    for (uint32_t i = 0; i < numUsers; i++) {
        user_t* userRecord = reinterpret_cast<user_t*>(buffer + (i * sizeof(user_t)));
        
        // Verificar si el usuario está en uso
        if (userRecord->isUsed != '1') {
            continue;
        }
        
        // Extraer nombre
        std::string username(userRecord->name, USER_NAME_SIZE);
        username.erase(std::remove(username.begin(), username.end(), FILLER), 
                      username.end());
        
        if (username.empty()) {
            continue;
        }
        
        // Crear AuthUser
        AuthUser authUser;
        authUser.username = username;
        
        // Convertir hash hexadecimal a bytes
        unsigned char hashBytes[32];
        std::string hashHex(userRecord->hash, USER_HASH_SIZE);
        hexToBytes(hashHex, hashBytes, 32);
        authUser.passwordHash.assign(reinterpret_cast<char*>(hashBytes), 32);
        
        // Convertir salt hexadecimal a bytes
        unsigned char saltBytes[crypto_pwhash_SALTBYTES];
        std::string saltHex(userRecord->salt, USER_SALT_SIZE);
        hexToBytes(saltHex, saltBytes, crypto_pwhash_SALTBYTES);
        authUser.salt.assign(reinterpret_cast<char*>(saltBytes), 
                            crypto_pwhash_SALTBYTES);
        
        authUser.isConnected = false;
        authUser.rank = userRecord->type;
        
        users[username] = authUser;
        
        std::cout << "Usuario cargado: " << username 
                  << " (tipo: " << userRecord->type << ")" << std::endl;
    }
    
    delete[] buffer;
    
    std::cout << "Total de usuarios cargados: " << users.size() << std::endl;
}

void AuthenticationServer::changePassword(const std::string& username,
                                         const std::string& newPassword) {
    std::lock_guard<std::mutex> lock(usersMutex);
    
    auto it = users.find(username);
    if (it == users.end()) {
        std::cout << "Error: Usuario '" << username << "' no existe" << std::endl;
        return;
    }
    
    if (newPassword.length() < 4) {
        std::cout << "Error: Nueva contraseña muy corta (mínimo 4 caracteres)" 
                  << std::endl;
        return;
    }
    
    // Generar nuevo salt
    unsigned char newSalt[crypto_pwhash_SALTBYTES];
    generateSalt(newSalt);
    
    // Hashear nueva contraseña
    unsigned char newHash[32];
    if (!hashPassword(newPassword, newSalt, newHash)) {
        std::cout << "Error: No se pudo hashear la nueva contraseña" << std::endl;
        return;
    }
    
    // Actualizar en memoria
    it->second.passwordHash.assign(reinterpret_cast<char*>(newHash), 32);
    it->second.salt.assign(reinterpret_cast<char*>(newSalt), 
                          crypto_pwhash_SALTBYTES);
    
    // Actualizar en el archivo
    uint32_t fileSize = fs->getFileSize("user_data.csv");
    uint32_t numUsers = fileSize / sizeof(user_t);
    
    char* buffer = new char[fileSize];
    uint32_t bytesRead = fileSize;
    
    if (!fs->readFile("user_data.csv", buffer, bytesRead)) {
        std::cerr << "Error al leer archivo de usuarios" << std::endl;
        delete[] buffer;
        return;
    }
    
    bool found = false;
    for (uint32_t i = 0; i < numUsers; i++) {
        user_t* userRecord = reinterpret_cast<user_t*>(buffer + (i * sizeof(user_t)));
        
        if (userRecord->isUsed != '1') continue;
        
        std::string recordName(userRecord->name, USER_NAME_SIZE);
        recordName.erase(std::remove(recordName.begin(), recordName.end(), FILLER),
                        recordName.end());
        
        if (recordName == username) {
            // Actualizar hash y salt
            hexLiterals(newHash, 32, userRecord->hash, USER_HASH_SIZE);
            hexLiterals(newSalt, crypto_pwhash_SALTBYTES, 
                       userRecord->salt, USER_SALT_SIZE);
            found = true;
            break;
        }
    }
    
    if (found) {
        // Escribir todo el archivo de vuelta
        if (fs->writeFile("user_data.csv", buffer, fileSize)) {
            std::cout << "Contraseña actualizada para '" << username << "'" 
                      << std::endl;
        } else {
            std::cerr << "Error al guardar cambios en el filesystem" << std::endl;
        }
    }
    
    delete[] buffer;
}

bool AuthenticationServer::deleteUser(const std::string& username) {
    std::lock_guard<std::mutex> lock(usersMutex);
    
    // Verificar que el usuario existe en memoria
    auto it = users.find(username);
    if (it == users.end()) {
        std::cout << "Error: Usuario '" << username << "' no existe" << std::endl;
        return false;
    }
    
    // Si el usuario está conectado, desconectarlo primero
    if (it->second.isConnected) {
        std::cout << "Advertencia: Usuario '" << username 
                  << "' está conectado, desconectando..." << std::endl;
        it->second.isConnected = false;
        counterMutex.wait();
        if (connectedUsersCount > 0) {
            connectedUsersCount--;
        }
        counterMutex.signal();
    }
    
    // Leer el archivo completo
    uint32_t fileSize = fs->getFileSize("user_data.csv");
    if (fileSize == 0) {
        std::cerr << "Error: Archivo de usuarios vacío" << std::endl;
        return false;
    }
    
    uint32_t numUsers = fileSize / sizeof(user_t);
    
    char* buffer = new char[fileSize];
    uint32_t bytesRead = fileSize;
    
    if (!fs->readFile("user_data.csv", buffer, bytesRead)) {
        std::cerr << "Error al leer archivo de usuarios" << std::endl;
        delete[] buffer;
        return false;
    }
    
    // Buscar y marcar el usuario como no usado
    bool found = false;
    for (uint32_t i = 0; i < numUsers; i++) {
        user_t* userRecord = reinterpret_cast<user_t*>(buffer + (i * sizeof(user_t)));
        
        if (userRecord->isUsed != '1') continue;
        
        std::string recordName(userRecord->name, USER_NAME_SIZE);
        recordName.erase(std::remove(recordName.begin(), recordName.end(), FILLER),
                        recordName.end());
        
        if (recordName == username) {
            // Marcar como en desuso
            userRecord->isUsed = '0';
            found = true;
            break;
        }
    }
    
    if (!found) {
        std::cout << "Error: Usuario '" << username 
                  << "' no encontrado en el archivo" << std::endl;
        delete[] buffer;
        return false;
    }
    
    // Escribir el archivo actualizado
    if (!fs->writeFile("user_data.csv", buffer, fileSize)) {
        std::cerr << "Error al guardar cambios en el filesystem" << std::endl;
        delete[] buffer;
        return false;
    }
    
    delete[] buffer;
    
    // Eliminar de la memoria
    users.erase(username);
    
    std::cout << "Usuario '" << username << "' eliminado exitosamente" << std::endl;
    return true;
}

void AuthenticationServer::changePermissions(const std::string& username,
                                            char newType,
                                            char newPermission) {
    std::lock_guard<std::mutex> lock(usersMutex);
    
    auto it = users.find(username);
    if (it == users.end()) {
        std::cout << "Error: Usuario '" << username << "' no existe" << std::endl;
        return;
    }
    
    // Actualizar en memoria
    it->second.rank = newType;
    
    // Actualizar en el archivo
    uint32_t fileSize = fs->getFileSize("user_data.csv");
    uint32_t numUsers = fileSize / sizeof(user_t);
    
    char* buffer = new char[fileSize];
    uint32_t bytesRead = fileSize;
    
    if (!fs->readFile("user_data.csv", buffer, bytesRead)) {
        std::cerr << "Error al leer archivo de usuarios" << std::endl;
        delete[] buffer;
        return;
    }
    
    bool found = false;
    for (uint32_t i = 0; i < numUsers; i++) {
        user_t* userRecord = reinterpret_cast<user_t*>(buffer + (i * sizeof(user_t)));
        
        if (userRecord->isUsed != '1') continue;
        
        std::string recordName(userRecord->name, USER_NAME_SIZE);
        recordName.erase(std::remove(recordName.begin(), recordName.end(), FILLER),
                        recordName.end());
        
        if (recordName == username) {
            // Actualizar permisos
            userRecord->type = newType;
            userRecord->permissions = newPermission;
            found = true;
            break;
        }
    }
    
    if (found) {
        // Escribir todo el archivo de vuelta
        if (fs->writeFile("user_data.csv", buffer, fileSize)) {
            std::cout << "Permisos actualizados para '" << username << "'" 
                      << std::endl;
        } else {
            std::cerr << "Error al guardar cambios en el filesystem" << std::endl;
        }
    }
    
    delete[] buffer;
}