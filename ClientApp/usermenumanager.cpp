#include "usermenumanager.h"
#include <iostream>
#include <string>

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
        QString online_tag = "";

        if (currentUser->getUser() == userInfo.user) {
            self_tag = " (Usted)";
        }

        if (userInfo.isConnected) {
            online_tag = "(ONL) ";
        }

        // Crear item con el nombre del usuario
        QString itemText = online_tag + QString::fromStdString(userInfo.user) + self_tag;
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

    UserInfo* user = getSelectedUserInfo(nullptr);

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

    // Obtener el texto del item y limpiar TODOS los tags
    QString itemText = selectedUser->text();

    // Limpiar tag de "Usted"
    itemText = itemText.replace(" (Usted)", "");

    // Limpiar tag de "ONL" (al inicio)
    itemText = itemText.replace("(ONL) ", "");

    // También limpiar espacios extras por si acaso
    itemText = itemText.trimmed();

    std::string username = itemText.toStdString();

    // Debug: imprimir el username que estamos buscando
    std::cout << "Buscando usuario: '" << username << "'" << std::endl;

    // Buscar en el vector
    for (auto& userInfo : *const_cast<std::vector<UserInfo>*>(users)) {
        if (userInfo.user == username) {
            std::cout << "Usuario encontrado: " << userInfo.user << std::endl;
            return &userInfo;
        }
    }

    std::cout << "Usuario no encontrado en la lista" << std::endl;
    return nullptr;
}
