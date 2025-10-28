#ifndef USERMENUMANAGER_H
#define USERMENUMANAGER_H

#include <QListWidgetItem>
#include "userdataqt.h"
#include <QPushButton>


class userMenuManager
{
public:
    userMenuManager();

    void setPointers(QListWidget* user_list,
    userDataQt* current);

    void updateUserList();

    void setSelectedUser(QListWidgetItem* user);

    QListWidgetItem* getSelectedUser();

    void hideDeleteButton(QPushButton* deleteButton);

private:
    QListWidget* user_list;
    userDataQt* currentUser;
    QListWidgetItem* selectedUser;
};

#endif // USERMENUMANAGER_H
