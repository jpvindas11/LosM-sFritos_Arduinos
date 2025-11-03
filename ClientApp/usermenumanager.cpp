#include "usermenumanager.h"

userMenuManager::userMenuManager() : selectedUser(nullptr)
{
}

void userMenuManager::updateUserList(QListWidget* userList,
                                     const std::vector<UserInfo>* users,
                                     userDataQt* currentUser) {
    if (!userList || !users || !currentUser) return;

    userList->clear();

    // Iterar sobre el vector de usuarios
    for (const auto& userInfo : *users) {
        // Agregar tag (Usted) si es el usuario actual
        QString self_tag = "";
        if (currentUser->getUser() == userInfo.user) {
            self_tag = " (Usted)";
        }

        // Crear item con el nombre del usuario
        QString itemText = QString::fromStdString(userInfo.user) + self_tag;
        QListWidgetItem *item = new QListWidgetItem(itemText, userList);
        userList->addItem(item);
    }
}

void userMenuManager::setSelectedUser(QListWidgetItem* user) {
    this->selectedUser = user;
}

QListWidgetItem* userMenuManager::getSelectedUser() {
    return this->selectedUser;
}

void userMenuManager::hideDeleteButton(QPushButton* deleteButton,
                                       userDataQt* currentUser) {
    if (!deleteButton || !currentUser) return;

    UserInfo* user = getSelectedUserInfo(nullptr); // Necesitarás pasar el vector
    if (user && currentUser->getUser() == user->user) {
        deleteButton->setEnabled(false);
    } else {
        deleteButton->setEnabled(true);
    }
}

UserInfo* userMenuManager::getSelectedUserInfo(const std::vector<UserInfo>* users) {
    if (selectedUser == nullptr || users == nullptr) {
        return nullptr;
    }

    // Obtener el texto del item y limpiar el tag "(Usted)"
    QString itemText = selectedUser->text();
    itemText = itemText.replace(" (Usted)", "");
    std::string username = itemText.toStdString();

    // Buscar en el vector
    for (auto& userInfo : *const_cast<std::vector<UserInfo>*>(users)) {
        if (userInfo.user == username) {
            return &userInfo;
        }
    }

    return nullptr;
}
