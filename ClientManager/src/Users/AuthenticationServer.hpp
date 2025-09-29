/// @copyright Los Más Fritos - 2025


#ifndef AUTHENTICATION_SERVER
#define AUTHENTICATION_SERVER

#include <sodium.h>
#include <string>
#include <unordered_map>
#include <mutex>

#include "FileSystem.hpp"
#include "Server.hpp"
#include "Semaphore.hpp"
#include "User.hpp"

/// @brief Estructura para datos de usuario almacenados en memoria
struct AuthUser {
  std::string username;
  std::string passwordHash;
  std::string salt;  ///< Salt único para este usuario
  bool isConnected;
};

class AuthenticationServer: public Server {
  DISABLE_COPY(AuthenticationServer);

 private:
  /// File system para guardar usuarios
  FileSystem* fs;
  /// Contador de usuarios conectados
  int connectedUsersCount;
  /// Semáforo para controlar acceso al contador
  Semaphore counterMutex;
  /// Mapa de usuarios registrados
  std::unordered_map<std::string, AuthUser> users;
  /// Mutex para proteger el mapa de usuarios
  std::mutex usersMutex;

  /// @brief Genera un salt aleatorio
  /// @param salt Buffer donde se almacenará el salt
  void generateSalt(unsigned char* salt);

  /// @brief Hashea una contraseña usando Argon2
  /// @param password Contraseña en texto plano
  /// @param salt Salt de usuario
  /// @param hash Buffer donde se almacenará el hash (debe ser al menos 32 bytes)
  /// @return true si el hash fue exitoso, false en caso contrario
  bool hashPassword(const std::string& password
                  , const unsigned char* salt
                  , unsigned char* hash);

  /// @brief Verifica una contraseña contra un hash usando salt
  /// @param password Contraseña en texto plano
  /// @param storedHash Hash almacenado
  /// @param storedSalt Salt almacenado
  /// @return true si la contraseña es correcta, false en caso contrario
  bool verifyPassword(const std::string& password
                    , const std::string& storedHash
                    , const std::string& storedSalt);

  /// @brief Procesa mensaje de login
  /// @param username Nombre de usuario
  /// @param password Contraseña
  /// @return Mensaje de respuesta
  std::string processLogin(const std::string& username
                         , const std::string& password);

  /// @brief Procesa mensaje de logout
  /// @param username Nombre de usuario
  /// @return Mensaje de respuesta
  std::string processLogout(const std::string& username);

  /// @brief Registra un nuevo usuario internamente del lado del servidor
  /// @param username Nombre de usuario
  /// @param password Contraseña
  /// @param type Tpo de usuario
  /// @param permission Permisos de usuario
  /// @return true si el registro fue exitoso, false en caso contrario
  bool registerUser(const std::string& username, const std::string& password,
                                                    char type, char permission);

  /// @brief Obtiene el número de usuarios conectados
  /// @return Mensaje con el número de usuarios conectados
  std::string processGetConnectedUsers();

  /// @brief Parsea un mensaje del cliente (solo LOGIN, LOGOUT, GET_CONNECTED)
  /// @param message Mensaje completo del cliente
  /// @param command Comando extraído
  /// @param username Usuario extraído
  /// @param password Contraseña extraída (si aplica)
  /// @return true si el parseo fue exitoso
  bool parseMessage(const std::string& message, std::string& command
                        , std::string& username, std::string& password);

 public:
  explicit AuthenticationServer();
  virtual ~AuthenticationServer();

  /// @brief Inicializa el servidor de autenticación
  /// @return 0 en caso de éxito, -1 en caso de error
  int initialize();

  /// @brief Procesa el mensaje recibido del cliente
  void processMessage() override;

  /// @brief Envía mensaje de respuesta al cliente
  void sendMessage() override;

  /// @brief Registra un nuevo usuario (función pública del servidor)
  /// @param username Nombre de usuario
  /// @param password Contraseña en texto plano
  /// @param type Tpo de usuario
  /// @param permission Permisos de usuario
  /// @return true si el registro fue exitoso, false en caso contrario
  bool addUser(const std::string& username, const std::string& password,
                                                    char type, char permission);

  /// @brief Obtiene información del salt de un usuario (para debugging)
  /// @param username Nombre del usuario
  /// @return Salt en formato hexadecimal o string vacío si no existe
  std::string getUserSaltHex(const std::string& username);

  void hexLiterals(const unsigned char* input, size_t inputLen,
                                                char* output, size_t outputLen);  
                                                
  void loadUsers();

  void processUsers(std::vector<std::string>& processUser, std::string& user);

  void hexLiterals(const unsigned char* input, size_t inputLen,
                                                char* output, size_t outputLen);  
                                                
  void loadUsers();

  void processUsers(std::vector<std::string>& processUser, std::string& user);

  bool status();

  std::unordered_map<std::string, AuthUser>* AuthenticationServer::getUserMap();
};

#endif
