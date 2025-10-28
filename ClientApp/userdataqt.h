#ifndef USERDATAQT_H
#define USERDATAQT_H

#include <string>

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
    int getRank();

private:
    std::string user;
    std::string password;
    std::string IP;
    int port;
    int rank;
};

#endif // USERDATAQT_H
