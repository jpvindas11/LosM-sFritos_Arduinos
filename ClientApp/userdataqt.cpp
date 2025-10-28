#include "userdataqt.h"

userDataQt::userDataQt() : user(""), password(""), rank(-1)
{

}

void userDataQt::setData(std::string user, std::string password, char rank) {
    this->user = user;
    this->password = password;

    switch(rank) {
        case 'o' : this->rank = UR_OWNER; break;
        case 'h' : this->rank = UR_HARDWAREMANAGER; break;
        case 's' : this->rank = UR_SOFTWAREMANAGER; break;
        case 'u' : this->rank = UR_USERMANAGER; break;
        case 'c' : this->rank = UR_CONSULTANT; break;
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
int userDataQt::getRank() { return this->rank; }
