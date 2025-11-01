#ifndef USERMENUMANAGER_H
#define USERMENUMANAGER_H

#include <QListWidgetItem>
#include <QListWidget>
#include <QPushButton>
#include <vector>
#include "userdataqt.h"
#include "User.hpp"

class userMenuManager
{
public:
    userMenuManager();
    void updateUserList(QListWidget* userList, const std::vector<UserInfo>* users, userDataQt* currentUser);
    void setSelectedUser(QListWidgetItem* user);
    QListWidgetItem* getSelectedUser();
    void hideDeleteButton(QPushButton* deleteButton, userDataQt* currentUser);
    UserInfo* getSelectedUserInfo(const std::vector<UserInfo>* users);

private:
    QListWidgetItem* selectedUser;
};

#endif // USERMENUMANAGER_H
