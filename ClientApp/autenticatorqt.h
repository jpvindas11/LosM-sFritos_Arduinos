#ifndef AUTENTICATORQT_H
#define AUTENTICATORQT_H

#include <string>
#include <vector>
#include "userdataqt.h"
#include "AuthenticationServer.hpp"

// Esta clase creo que se podria quitar cuando ustedes
// Agreguen el check verdadero de la contraseña
// También pueden usar el tryLogin() para que llame
// A la función que ustedes hagan, entonces no hace falta cambiar mucho

class autenticatorQt
{
public:
    autenticatorQt();

    bool tryLogin(std::string user, std::string password, AuthenticationServer* authServer);
private:
    // Users test
    std::vector<userDataQt> users;

};

#endif // AUTENTICATORQT_H
