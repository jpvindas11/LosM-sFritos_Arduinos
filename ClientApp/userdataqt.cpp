#include "userdataqt.h"

userDataQt::userDataQt() : user(""), password(""), rank(-1)
{

}

void userDataQt::setData(std::string user, std::string password, char rank) {
    this->user = user;
    this->password = password;

    switch(rank) {
        case USER_OWNER : this->rank = UR_OWNER; break;
        case USER_HARDWAREMANAGER : this->rank = UR_HARDWAREMANAGER; break;
        case USER_SOFTWAREMANAGER : this->rank = UR_SOFTWAREMANAGER; break;
        case USER_USERMANAGER : this->rank = UR_USERMANAGER; break;
        case USER_CONSULTANT : this->rank = UR_CONSULTANT; break;
        default: break;
    }
}

void userDataQt::setNetwork(std::string IP, int port) {
    this->IP = IP;
    this->port = port;
}

std::string userDataQt::getUser() { return this->user; }
std::string userDataQt::getPass() { return this->password; }
std::string userDataQt::getIP() { return this->IP; }
int userDataQt::getPort() { return this->port; }
uint8_t userDataQt::getRank() { return this->rank; }
