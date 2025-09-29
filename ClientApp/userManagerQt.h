#ifndef USERMANAGERQT_H
#define USERMANAGERQT_H

#include <string>

// Enum para los rangos de usuario
enum userRank {
    UR_USERMANAGER,
    UR_HARDWAREMANAGER,
    UR_OWNER
};

// Estructura para los datos del usuario
typedef struct userData {
    std::string username;
    std::string password;
    int rank;
} userData;

#endif // USERMANAGERQT_H
