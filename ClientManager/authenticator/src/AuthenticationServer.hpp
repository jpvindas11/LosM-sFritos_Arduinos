/// @copyright Los Más Fritos - 2025

#ifndef AUTHENTICATION_SERVER
#define AUTHENTICATION_SERVER

#include <sodium.h>
#include <string>
#include <unordered_map>
#include <mutex>
#include <regex>

#include "../../FileSystem/src/FileSystem.hpp"
#include "../../Util/Semaphore.hpp"
#include "../../Util/Socket.hpp"
#include "../../Util/User.hpp"
#include "../../Util/IPConstants.hpp"
#include "../../Util/UDPSocket.hpp"
#include "../../Util/ServerDiscover.hpp"
#include "../../Util/ServerDiscoveryPoint.hpp"

/// @brief Estructura para datos de usuario almacenados en memoria
struct AuthUser {
    std::string username;
    std::string passwordHash;
    std::string salt;
    char rank;
    bool isConnected;
};

enum class RegisterResult {
    SUCCESS = 0,
    USER_EXISTS = 1,
    INVALID_PASSWORD = 2,
    HASH_ERROR = 3,
    FILE_ERROR = 4
};

enum class ChangePasswordResult {
    SUCCESS = 0,
    USER_NOT_FOUND = 1,
    INVALID_PASSWORD = 2,
    HASH_ERROR = 3,
    FILE_READ_ERROR = 4,
    FILE_WRITE_ERROR = 5
};

class AuthenticationServer {
    DISABLE_COPY(AuthenticationServer);

private:
    FileSystem* fs;
    int connectedUsersCount;
    Semaphore counterMutex;
    std::unordered_map<std::string, AuthUser> users;
    std::mutex usersMutex;
    std::mutex requestMutex;
    Socket listeningSocket;
    std::string serverIP;
    int listeningPort;
    std::regex passwordPattern;

    ServerDiscoveryPoint* discoveryPoint;

    void generateSalt(unsigned char* salt);
    bool hashPassword(const std::string& password, const unsigned char* salt, unsigned char* hash);
    bool verifyPassword(const std::string& password, const std::string& storedHash, const std::string& storedSalt);

    genMessage processLoginRequest(const authLoginReq& req);
    genMessage processLogoutRequest(const authLogout& req);
    genMessage processCreateUserRequest(const authCreateUser& req);
    genMessage processDeleteUserRequest(const authDeleteUser& req);
    genMessage processModPassRequest(const authModifyUserPass& req);
    genMessage processModRankRequest(const authModifyUserRank& req);
    genMessage processUsersInfoRequest(const authRequestUsers& req);

    RegisterResult registerUser(const std::string& username, const std::string& password, 
                           char type, char permission);
    bool updateUserInFile(const std::string& username, std::function<void(user_t*)> updateFn);

    bool storeUsersInVector(std::vector<UserInfo>* usersVec);

    int openConnectionRequestSocket(std::string ip, int port);

    void hexLiterals(const unsigned char* input, size_t inputLen, char* output, size_t outputLen);
    void hexToBytes(const std::string& hexString, unsigned char* output, size_t outputLen);

    void sendUserLog(const std::string& username, const std::string& action);

    std::string getCurrentTimestamp();

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
    ChangePasswordResult changePassword(const std::string& username, const std::string& newPassword);
    bool deleteUser(const std::string& username);
    void changePermissions(const std::string& username, char newType, char newPermission);

    void printUsers();
    std::unordered_map<std::string, AuthUser>* getUserMap();
};

#endif