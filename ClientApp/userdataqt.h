#ifndef USERDATAQT_H
#define USERDATAQT_H

#include <string>
#include "inttypes.h"
#include "User.hpp"

enum USER_RANK {
    UR_CONSULTANT,
    UR_USERMANAGER,
    UR_HARDWAREMANAGER,
    UR_SOFTWAREMANAGER,
    UR_OWNER,
};

class userDataQt
{
public:
    userDataQt();

    void setData(std::string user, std::string password, char rank);
    void setNetwork(std::string IP, int port);

    std::string getUser();
    std::string getPass();
    std::string getIP();
    int getPort();
    uint8_t getRank();

private:
    std::string user;
    std::string password;
    std::string IP;
    int port;
    uint8_t rank;
};

#endif // USERDATAQT_H
