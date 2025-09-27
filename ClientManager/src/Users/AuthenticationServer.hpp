#ifndef AUTHENTICATION_SERVER
#define AUTHENTICATION_SERVER

#include <mutex>
#include <string>
#include <vector>
#include <set>
#include "Server.hpp"
#include "common.hpp"
#include "FileSystem.hpp"

struct UserData
{
  std::string username;
  std::string salt;
  std::string hash;
};

enum class AuthResult {
  SUCCESS,
  USER_NOT_FOUND,
  INVALID_PASSWORD,
  ERROR
};


class AuthenticationServer: public Server {
  DISABLE_COPY(AuthenticationServer);

 private:
  FileSystem& fileSystem;
  std::vector<UserData> users;
  std::set<std::string> connectedUsers;
  mutable std::mutex usersMutex;
  mutable std::mutex connectedUsersMutex;
  std::string userDataFile = "user_data.csv";
  bool serverRunning;

  

 public:
  explicit AuthenticationServer(FileSystem& fs);
  virtual ~AuthenticationServer();

  void processMessage() override;
  void sendMessage() override;

  void startServer();
  void stopServer();

  int getConnectedUsersCount() const;
  bool isServerRunning() const;

 private:
  std::string hashPassword(const std::string& password, const std::string& salt);
  std::string generateSalt();
  bool verifyPassword(const std::string& password, const std::string& salt, const std::string& hash);

  std::pair<std::string, std::string> decryptCredentials(const std::string& message);
  std::string encryptMessage(const std::string& response);

  bool loadUserData();
  bool saveUserData();
  bool findUser(const std::string& username, std::string& salt, std::string& hash);
  bool addUser(const std::string& username, const std::string& hash, const std::string& salt);

  AuthResult processLogin(const std::string& username, const std::string& password);
  AuthResult processRegistration(const std::string& username, const std::string& password);

  void registerUserConnected(const std::string& username);
  void unregisterUserConnected(const std::string& username);
  bool isUserConnected(const std::string& username) const;

};

#endif // AUTHENTICATION_SERVER