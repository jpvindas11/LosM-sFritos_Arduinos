/// @copyright Los Más Fritos - 2025

/// @copyright Los Más Fritos - 2025

#include "AuthenticationServer.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <iomanip>

#include "FileSystem.hpp"

AuthenticationServer::AuthenticationServer(FileSystem* fileSystem)
    : Server(), fs(fileSystem), connectedUsersCount(0), counterMutex(1) {
}

AuthenticationServer::~AuthenticationServer() {
  // Cleanup automático por destructores
  delete fs;
}

int AuthenticationServer::initialize() {
  // Inicializar libsodium
  if (sodium_init() < 0) {
    std::cerr << "Error: No se pudo inicializar libsodium" << std::endl;
    return -1;
  }
  return 0;
}

void AuthenticationServer::generateSalt(unsigned char* salt) {
  randombytes_buf(salt, crypto_pwhash_SALTBYTES);
}

bool AuthenticationServer::hashPassword(const std::string& password
    , const unsigned char* salt
    , unsigned char* hash) {
  // Usar Argon2id con salt personalizado
  if (crypto_pwhash(hash, 32, password.c_str(), password.length(), salt
  , crypto_pwhash_OPSLIMIT_INTERACTIVE
  , crypto_pwhash_MEMLIMIT_INTERACTIVE
  , crypto_pwhash_ALG_DEFAULT) != 0) {
    return false;
  }
  return true;
}

bool AuthenticationServer::verifyPassword(const std::string& password
    , const std::string& storedHash
    , const std::string& storedSalt) {
    // Convertir salt de string a bytes
    unsigned char salt[crypto_pwhash_SALTBYTES];
    if (storedSalt.length() != crypto_pwhash_SALTBYTES) {
        return false;
    }
    memcpy(salt, storedSalt.c_str(), crypto_pwhash_SALTBYTES);
    // Generar hash con la contraseña ingresada y el salt almacenado
    unsigned char hash[32];
    if (!hashPassword(password, salt, hash)) {
      return false;
    }
    // Comparar hashes
    if (storedHash.length() != 32) {
      return false;
    }
    return sodium_memcmp(hash, storedHash.c_str(), 32) == 0;
}

bool AuthenticationServer::parseMessage(const std::string& message
    , std::string& command
    , std::string& username, std::string& password) {
  std::istringstream iss(message);
  std::string token;
  // Formato esperado: "LOGIN username password", "LOGOUT username"
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

std::string AuthenticationServer::processLogin(const std::string& username
    , const std::string& password) {
  std::lock_guard<std::mutex> lock(usersMutex);
  // Verificar si el usuario existe
  auto it = users.find(username);
  if (it == users.end()) {
      return "ERROR Usuario no existe";
  }
  // Verificar si ya está conectado
  if (it->second.isConnected) {
      return "ERROR Usuario ya conectado";
  }
  // Verificar contraseña con salt
  if (!verifyPassword(password, it->second.passwordHash, it->second.salt)) {
      return "ERROR Contraseña incorrecta";
  }
  // Marcar como conectado
  it->second.isConnected = true;
  counterMutex.wait();
  connectedUsersCount++;
  counterMutex.signal();
  return "SUCCESS Login exitoso";
}

std::string AuthenticationServer::processLogout(const std::string& username) {
  std::lock_guard<std::mutex> lock(usersMutex);
  // Verificar si el usuario existe
  auto it = users.find(username);
  if (it == users.end()) {
    return "ERROR Usuario no existe";
  }
  // Verificar si está conectado
  if (!it->second.isConnected) {
    return "ERROR Usuario no está conectado";
  }
  // Marcar como desconectado
  it->second.isConnected = false;
  // Decrementar contador de usuarios conectados (thread-safe)
  counterMutex.wait();
  if (connectedUsersCount > 0) {
    connectedUsersCount--;
  }
  counterMutex.signal();
  return "SUCCESS Logout exitoso";
}

bool AuthenticationServer::registerUser(const std::string& username
    , const std::string& password) {
  std::lock_guard<std::mutex> lock(usersMutex);
  // Verificar si el usuario ya existe
  if (users.find(username) != users.end()) {
    std::cout << "Error: Usuario '"
              << username
              << "' ya existe"
              << std::endl;
    return false;
  }
  // Validar contraseña (mínimo 4 caracteres para simplificar)
  if (password.length() < 4) {
    std::cout << "Error: Contraseña muy corta para usuario '"
              << username
              << "' (mínimo 4 caracteres)"
              << std::endl;
    return false;
  }
  // Generar salt único para este usuario
  unsigned char salt[crypto_pwhash_SALTBYTES];
  generateSalt(salt);
  // Hashear contraseña con el salt generado
  unsigned char hash[32];
  if (!hashPassword(password, salt, hash)) {
    std::cout << "Error: No se pudo hashear la contraseña para usuario '"
              << username
              << "'"
              << std::endl;
    return false;
  }
  // Crear nuevo usuario
  AuthUser newUser;
  newUser.username = username;
  newUser.passwordHash = std::string(reinterpret_cast<char*>(hash), 32);
  newUser.salt = std::string(reinterpret_cast<char*>(salt)
      , crypto_pwhash_SALTBYTES);
  newUser.isConnected = false;
  users[username] = newUser;
  std::cout << "Usuario '"
            << username
            << "' registrado exitosamente por el servidor"
            << std::endl;
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
  // Parsear mensaje
  if (!parseMessage(message, command, username, password)) {
    response = "ERROR Formato de mensaje inválido";
  } else {
    // Procesar comando
    if (command == "LOGIN") {
      response = processLogin(username, password);
    } else if (command == "LOGOUT") {
      response = processLogout(username);
    } else {
      response = "ERROR Comando no reconocido";
    }
  }
  // Guardar respuesta para envío
  message = response;
  std::cout << "Respuesta: "
            << response
            << std::endl;
}

void AuthenticationServer::sendMessage() {
  std::cout << "Enviando respuesta al cliente: "
            << message
            << std::endl;
}

bool AuthenticationServer::addUser(const std::string& username
    , const std::string& password) {
  return registerUser(username, password);
}

std::string AuthenticationServer::getUserSaltHex(const std::string& username) {
  std::lock_guard<std::mutex> lock(usersMutex);
  auto it = users.find(username);
  if (it == users.end()) {
    return "";
  }
  // Convertir salt a hexadecimal
  std::stringstream ss;
  for (size_t i = 0; i < it->second.salt.length()
    && i < crypto_pwhash_SALTBYTES; ++i) {
    ss << std::hex
       << std::setfill('0')
       << std::setw(2)
       << static_cast<unsigned int>
         (static_cast<unsigned char>(it->second.salt[i]));
  }
  return ss.str();
}

bool AuthenticationServer::status() {
  if(message == "SUCCESS") {
      return true;
  }
  return false;
}
