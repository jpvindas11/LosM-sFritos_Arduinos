#ifndef USERMENUMANAGER_H
#define USERMENUMANAGER_H

#include <QListWidgetItem>
#include "userdataqt.h"
#include "AuthenticationServer.hpp"

class userMenuManager
{
public:
    userMenuManager();

    void setPointers(QListWidget* user_list,
    std::unordered_map<std::string, AuthUser>* users,
    userDataQt* current);

    void updateUserList();

    void setSelectedUser(QListWidgetItem* user);

    QListWidgetItem* getSelectedUser();

private:
    QListWidget* user_list;
    std::unordered_map<std::string, AuthUser>* users;
    userDataQt* currentUser;
    QListWidgetItem* selectedUser;
};

#endif // USERMENUMANAGER_H
