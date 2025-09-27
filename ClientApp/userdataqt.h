#ifndef USERDATAQT_H
#define USERDATAQT_H

#include <string>

enum USER_RANK {
    UR_USERMANAGER,
    UR_HARDWAREMANAGER,
    UR_OWNER,
};

class userDataQt
{
public:
    userDataQt();

    void setData(std::string user, std::string password, int rank);

    std::string getUser();
    std::string getPass();
    int getRank();

private:
    std::string user;
    std::string password;
    int rank;
};

#endif // USERDATAQT_H
