#include "usermenumanager.h"

userMenuManager::userMenuManager()
{

}

void userMenuManager::setPointers(QListWidget* user_list,
std::unordered_map<std::string, AuthUser>* users,
userDataQt* current) {
    this->user_list = user_list;
    this->users = users;
    this->currentUser = current;
}

void userMenuManager::updateUserList() {
    user_list->clear();

    // Open user scroll
    for (const auto &[key, usuario] : *users) {
        // Add (Usted) to name
        QString self_tag = "";
        if (currentUser->getUser() == key) self_tag = " (Usted)";

        QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(key) + self_tag, user_list);
        user_list->addItem(item);
    }
}

void userMenuManager::setSelectedUser(QListWidgetItem* user) {
    this->selectedUser = user;
}

QListWidgetItem* userMenuManager::getSelectedUser() {
    return this->selectedUser;
}
