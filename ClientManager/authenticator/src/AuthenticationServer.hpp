/// @copyright Los Más Fritos - 2025

#ifndef AUTHENTICATION_SERVER
#define AUTHENTICATION_SERVER

#include <sodium.h>
#include <string>
#include <unordered_map>
#include <mutex>

#include "../../FileSystem/src/FileSystem.hpp"
#include "../../Util/Semaphore.hpp"
#include "../../Util/Socket.hpp"
#include "../../Util/User.hpp"

/// @brief Estructura para datos de usuario almacenados en memoria
struct AuthUser {
    std::string username;
    std::string passwordHash;
    std::string salt;
    char rank;
    bool isConnected;
};

class AuthenticationServer {
    DISABLE_COPY(AuthenticationServer);

private:
    FileSystem* fs;
    int connectedUsersCount;
    Semaphore counterMutex;
    std::unordered_map<std::string, AuthUser> users;
    std::mutex usersMutex;
    Socket listeningSocket;
    std::string serverIP;
    int listeningPort;

    void generateSalt(unsigned char* salt);
    bool hashPassword(const std::string& password, const unsigned char* salt, unsigned char* hash);
    bool verifyPassword(const std::string& password, const std::string& storedHash, const std::string& storedSalt);

    genMessage processLoginRequest(const authLoginReq& req);

    bool registerUser(const std::string& username, const std::string& password, char type, char permission);
    bool updateUserInFile(const std::string& username, std::function<void(user_t*)> updateFn);

    int openConnectionRequestSocket(std::string ip, int port);

    void hexLiterals(const unsigned char* input, size_t inputLen, char* output, size_t outputLen);
    void hexToBytes(const std::string& hexString, unsigned char* output, size_t outputLen);

public:
    explicit AuthenticationServer();
    virtual ~AuthenticationServer();

    int initialize();
    void loadUsers();

    int listenForConnections(std::string ip, int port);
    int acceptConnectionRequest();
    void acceptAllConnections();
    void handleClientConnection(int clientSocket);
    void listenForever(std::string ip, int port);

    bool addUser(const std::string& username, const std::string& password, char type, char permission);
    void changePassword(std::string& username, const std::string& newPassword);
    bool deleteUser(const std::string& username);
    void changePermissions(std::string& username, char newType, char newPermission);

    void printUsers();
    std::unordered_map<std::string, AuthUser>* getUserMap();
};

#endif