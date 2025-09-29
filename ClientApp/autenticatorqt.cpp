#include "autenticatorqt.h"
#include <iostream>
#include <ostream>

autenticatorQt::autenticatorQt()
{
    // Usar inicialización de estructura en lugar de constructor
    userDataQt user;
    user.setData("carlos", "a", UR_USERMANAGER);
    this->users.push_back(user);

    user.setData("mario", "b", UR_HARDWAREMANAGER);
    this->users.push_back(user);

    user.setData("dora", "c", UR_OWNER);
    this->users.push_back(user);
}

// De mentiras
// Pueden poner dentro de esta funcion la de autenticacion que ustedes hicieron

bool autenticatorQt::tryLogin(std::string user, std::string password, AuthenticationServer* authServer) {
    std::string loginMessage = "LOGIN " + user + " " + password;
    authServer->setMessage(loginMessage);
    authServer->processMessage();
    authServer->sendMessage();
    return authServer->status();
}

