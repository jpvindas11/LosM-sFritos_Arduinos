/// @copyright Los Más Fritos - 2025

#include "AuthenticationServer.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <iomanip>
#include <vector>

AuthenticationServer::AuthenticationServer()
    : Server(), connectedUsersCount(0), counterMutex(1) {
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

bool AuthenticationServer::parseMessage(const std::string& message,
                                       std::string& command,
                                       std::string& username,
                                       std::string& password) {
    std::istringstream iss(message);
    
    if (!(iss >> command)) {
        return false;
    }
    
    if (command == "LOGIN") {
        if (!(iss >> username >> password)) {
            return false;
        }
    } else if (command == "LOGOUT") {
        if (!(iss >> username)) {
            return false;
        }
    } else {
        return false;
    }
    return true;
}

std::string AuthenticationServer::processLogin(const std::string& username,
                                              const std::string& password) {
    std::lock_guard<std::mutex> lock(usersMutex);
    
    auto it = users.find(username);
    if (it == users.end()) {
        return "ERROR Usuario no existe";
    }
    
    if (it->second.isConnected) {
        return "ERROR Usuario ya conectado";
    }
    
    if (!verifyPassword(password, it->second.passwordHash, it->second.salt)) {
        return "ERROR Contraseña incorrecta";
    }
    
    it->second.isConnected = true;
    counterMutex.wait();
    connectedUsersCount++;
    counterMutex.signal();
    
    return "SUCCESS";
}

std::string AuthenticationServer::processLogout(const std::string& username) {
    std::lock_guard<std::mutex> lock(usersMutex);
    
    auto it = users.find(username);
    if (it == users.end()) {
        return "ERROR Usuario no existe";
    }
    
    if (!it->second.isConnected) {
        return "ERROR Usuario no está conectado";
    }
    
    it->second.isConnected = false;
    counterMutex.wait();
    if (connectedUsersCount > 0) {
        connectedUsersCount--;
    }
    counterMutex.signal();
    
    return "SUCCESS Logout exitoso";
}

bool AuthenticationServer::registerUser(const std::string& username,
                                       const std::string& password,
                                       char type,
                                       char permission) {
    std::lock_guard<std::mutex> lock(usersMutex);
    
    if (users.find(username) != users.end()) {
        std::cout << "Error: Usuario '" << username << "' ya existe" << std::endl;
        return false;
    }
    
    if (password.length() < 4) {
        std::cout << "Error: Contraseña muy corta para usuario '" 
                  << username << "' (mínimo 4 caracteres)" << std::endl;
        return false;
    }
    
    // Generar salt único
    unsigned char salt[crypto_pwhash_SALTBYTES];
    generateSalt(salt);
    
    // Hashear contraseña
    unsigned char hash[32];
    if (!hashPassword(password, salt, hash)) {
        std::cout << "Error: No se pudo hashear la contraseña para usuario '" 
                  << username << "'" << std::endl;
        return false;
    }
    
    // Crear estructura user_t para guardar en el filesystem
    user_t user;
    memset(&user, 0, sizeof(user_t));
    
    user.isUsed = '1';
    
    // Copiar nombre y rellenar con FILLER
    memset(user.name, FILLER, USER_NAME_SIZE);
    size_t nameLen = std::min(username.length(), (size_t)USER_NAME_SIZE);
    memcpy(user.name, username.c_str(), nameLen);
    
    // Convertir hash y salt a hexadecimal
    hexLiterals(hash, 32, user.hash, USER_HASH_SIZE);
    hexLiterals(salt, crypto_pwhash_SALTBYTES, user.salt, USER_SALT_SIZE);
    
    user.type = type;
    user.permissions = permission;
    
    // Timestamp
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
    
    // Agregar usuario al archivo usando el nuevo FileSystem
    if (!fs->appendFile("user_data.csv", 
                       reinterpret_cast<const char*>(&user), 
                       sizeof(user_t))) {
        std::cout << "Error: No se pudo escribir usuario en el filesystem" << std::endl;
        return false;
    }
    
    // Crear usuario en memoria
    AuthUser newUser;
    newUser.username = username;
    newUser.passwordHash.assign(reinterpret_cast<char*>(hash), 32);
    newUser.salt.assign(reinterpret_cast<char*>(salt), crypto_pwhash_SALTBYTES);
    newUser.isConnected = false;
    newUser.rank = type;
    
    users[username] = newUser;
    
    std::cout << "Usuario '" << username 
              << "' registrado exitosamente" << std::endl;
    
    return true;
}

std::string AuthenticationServer::processGetConnectedUsers() {
    counterMutex.wait();
    int count = connectedUsersCount;
    counterMutex.signal();
    return "CONNECTED_USERS " + std::to_string(count);
}

void AuthenticationServer::processMessage() {
    std::string command, username, password;
    std::string response;
    
    std::cout << "Procesando mensaje: " << message << std::endl;
    
    if (!parseMessage(message, command, username, password)) {
        response = "ERROR Formato de mensaje inválido";
    } else {
        if (command == "LOGIN") {
            response = processLogin(username, password);
        } else if (command == "LOGOUT") {
            response = processLogout(username);
        } else {
            response = "ERROR Comando no reconocido";
        }
    }
    
    message = response;
    std::cout << "Respuesta: " << response << std::endl;
}

void AuthenticationServer::sendMessage() {
    std::cout << "Enviando respuesta al cliente: " << message << std::endl;
}

bool AuthenticationServer::addUser(const std::string& username,
                                   const std::string& password,
                                   char type,
                                   char permission) {
    return registerUser(username, password, type, permission);
}

std::string AuthenticationServer::getUserSaltHex(const std::string& username) {
    std::lock_guard<std::mutex> lock(usersMutex);
    
    auto it = users.find(username);
    if (it == users.end()) {
        return "";
    }
    
    std::stringstream ss;
    for (size_t i = 0; i < it->second.salt.length() && 
         i < crypto_pwhash_SALTBYTES; ++i) {
        ss << std::hex << std::setfill('0') << std::setw(2)
           << static_cast<unsigned int>(
               static_cast<unsigned char>(it->second.salt[i]));
    }
    return ss.str();
}

bool AuthenticationServer::status() {
    return (message == "SUCCESS");
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

void AuthenticationServer::processUsers(std::vector<std::string>& processUser,
                                       std::string& user) {
    // Esta función ya no es necesaria con el nuevo sistema
    // pero la mantengo por compatibilidad
    std::string username = user.substr(1, 20);
    username.erase(std::remove(username.begin(), username.end(), '*'), 
                  username.end());
    processUser.push_back(username);
    
    std::string hash = user.substr(21, 64);
    processUser.push_back(hash);
    
    std::string salt = user.substr(85, 32);
    processUser.push_back(salt);
    
    std::string type = user.substr(117, 1);
    processUser.push_back(type);
    
    std::string typePermissions = user.substr(118, 1);
    processUser.push_back(typePermissions);
}

void AuthenticationServer::printUsers() {
    std::cout << "\n=== USUARIOS REGISTRADOS ===" << std::endl;
    std::cout << "Total: " << users.size() << std::endl;
    std::cout << "Conectados: " << connectedUsersCount << std::endl;
    std::cout << "----------------------------" << std::endl;
    
    for (const auto& pair : users) {
        const AuthUser& user = pair.second;
        std::cout << "Usuario: " << user.username 
                  << " | Tipo: " << user.rank
                  << " | Estado: " << (user.isConnected ? "Conectado" : "Desconectado")
                  << std::endl;
    }
    std::cout << "============================\n" << std::endl;
}

void AuthenticationServer::saveUsers() {
    // El nuevo FileSystem maneja la persistencia automáticamente
    // Esta función ya no es necesaria, pero la mantengo por compatibilidad
    std::cout << "FileSystem guardado automáticamente" << std::endl;
}

void AuthenticationServer::changePassword(std::string& username,
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

void AuthenticationServer::changePermissions(std::string& username,
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