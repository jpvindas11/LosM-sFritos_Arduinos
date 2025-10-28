#include "usermenumanager.h"
userMenuManager::userMenuManager()
{

}
/*
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

void userMenuManager::hideDeleteButton(QPushButton *deleteButton) {
    // Hide delete button if its self

    AuthUser* user = getSelectedAuthUser();

    if (this->currentUser->getUser() == user->username) {
        deleteButton->setEnabled(false);
    } else deleteButton->setEnabled(true);
}
QListWidgetItem* userMenuManager::getSelectedUser() {
    return this->selectedUser;
}

AuthUser* userMenuManager::getSelectedAuthUser() {
    if (selectedUser == nullptr) {
        return nullptr;
    }

    QString itemText = selectedUser->text();
    itemText = itemText.replace(" (Usted)", "");
    std::string username = itemText.toStdString();

    auto it = users->find(username);
    if (it != users->end()) {
        return &(it->second);
    }

    return nullptr;
}
*/
