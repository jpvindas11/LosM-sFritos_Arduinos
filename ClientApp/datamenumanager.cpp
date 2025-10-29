#include "datamenumanager.h"

dataMenuManager::dataMenuManager()
{

}

void dataMenuManager::updateList() {
    user_list->clear();

}

void dataMenuManager::setSelectedArduino(QListWidgetItem* user) {
    this->selectedArduino = user;
}

void dataMenuManager::turnOff() {
    if (!this->selectedArduino) return;
}

void dataMenuManager::getData() {

}
