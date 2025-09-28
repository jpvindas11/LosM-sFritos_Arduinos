#include "AuthenticationServer.hpp"
#include "FileSystem.hpp"
#include <iostream>
#include <string>

int no_main() {
    // Crear e inicializar el servidor de autenticación
    FileSystem* filesystem = new FileSystem();
    filesystem->createFile("user_data.csv");
    AuthenticationServer* authServer = new AuthenticationServer(filesystem);
    if (authServer->initialize() != 0) {
        std::cerr << "Error al inicializar el servidor de autenticación" << std::endl;
        return -1;
    }

    //Registrar usuario de prueba y agregar al fileSystem
    std::string user = "admin";
    char buffer[100];
    authServer->addUser(user, "admin123");
    std::string getSalt = authServer->getUserSaltHex(user);
    filesystem->write("user_data.csv", 0, user.size()+getSalt.size(), (user + "," + getSalt).c_str());
    filesystem->read("user_data.csv", 0, 100, buffer); // Leer para verificar escritura
    std::cout << "Contenido de user_data.csv: " << buffer << std::endl;
    authServer->addUser("user1", "password1");
    authServer->addUser("user2", "password2");
    std::string user1 = "user1";
    char buffer1[100];
    authServer->addUser(user1, "admin123");
    std::string getSalt1 = authServer->getUserSaltHex(user1);
    filesystem->write("user_data.csv", 0, user1.size()+getSalt1.size(), (user1 + "," + getSalt1).c_str());
    filesystem->read("user_data.csv", 0, 100, buffer1); // Leer para verificar escritura
    filesystem->printDirectory();
    filesystem->printUnidad();

    // Login de prueba
    std::string loginUser = "admin";
    std::string loginPass = "admin123";
    std::string loginMessage = "LOGIN " + loginUser + " " + loginPass;
    authServer->setMessage(loginMessage);
    authServer->processMessage();
    authServer->sendMessage();
    
    return 0;
}
