#include "userdataqt.h"

userDataQt::userDataQt() : user(""), password(""), rank(-1)
{

}

void userDataQt::setData(std::string user, std::string password, int rank) {
    this->user = user;
    this->password = password;
    this->rank = rank;
}

std::string userDataQt::getUser() { return this->user; }
std::string userDataQt::getPass() { return this->password; }
int userDataQt::getRank() { return this->rank; }
