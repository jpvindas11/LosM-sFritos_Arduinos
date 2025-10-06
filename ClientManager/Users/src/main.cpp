#include "AuthenticationServer.hpp"
#include "FileSystem.hpp"
#include <iostream>
#include <string>

int main() {
  // Crear e inicializar el servidor de autenticación
  AuthenticationServer* authServer = new AuthenticationServer();
  if (authServer->initialize() != 0) {
    std::cerr << "Error al inicializar el servidor de autenticación" << std::endl;
    return -1;
  }
  //Registrar usuario de prueba y agregar al fileSystem
  std::string user = "admin";
  char buffer[512];
  authServer->addUser(user, "user123", 1, 1);
  std::string getSalt = authServer->getUserSaltHex(user);
  authServer->printUsers();
  delete authServer;

  return 0;
}
