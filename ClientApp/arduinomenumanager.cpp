#include "arduinomenumanager.h"

arduinoMenuManager::arduinoMenuManager()
{

}
/*
void arduinoMenuManager::setPointers(std::vector<ArduinoSimulation> *arduinos,
    QListWidget* user_list) {
    this->p_arduinos = arduinos;
    this->user_list = user_list;
}*/

void arduinoMenuManager::updateList() {
    user_list->clear();

}

void arduinoMenuManager::setSelectedArduino(QListWidgetItem* user) {
    this->selectedArduino = user;
}

void arduinoMenuManager::turnOff() {
    if (!this->selectedArduino) return;

}
