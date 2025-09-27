#include "autenticatorqt.h"

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

userDataQt autenticatorQt::tryLogin(std::string user, std::string password) {
    for (size_t i = 0; i < this->users.size(); ++i) {
        // Cambiar 'user' por 'username' para coincidir con la nueva definición
        if (this->users[i].getUser() == user && this->users[i].getPass() == password) {
            return this->users[i];
        }
    }
    // Struct vacío para fallo - usar inicialización de lista
    userDataQt clear;
    return clear;
}

