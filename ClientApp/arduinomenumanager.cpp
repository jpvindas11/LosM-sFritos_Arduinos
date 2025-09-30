#include "arduinomenumanager.h"

arduinoMenuManager::arduinoMenuManager()
{

}

void arduinoMenuManager::setPointers(std::vector<ArduinoSimulation> *arduinos,
    QListWidget* user_list) {
    this->p_arduinos = arduinos;
    this->user_list = user_list;
}

void arduinoMenuManager::updateList() {
    user_list->clear();

    for (ArduinoSimulation& arduino : *p_arduinos) {
        QString key = QString::fromStdString(arduino.getID());
        QString type = QString::fromStdString(arduino.arduinoType());
        QString connected = "false";

        if (arduino.getConnectionStatus()) { connected = "true"; }

        QListWidgetItem *item = new QListWidgetItem(key + " / Type: " + type + " / " + "Connected: " + connected, user_list);
        user_list->addItem(item);
    }
}

void arduinoMenuManager::setSelectedArduino(QListWidgetItem* user) {
    this->selectedArduino = user;
}

void arduinoMenuManager::turnOff() {
    if (!this->selectedArduino) return;

    int index = user_list->row(selectedArduino);
    if (index >= 0 && index < static_cast<int>(p_arduinos->size())) {
        if ((*p_arduinos)[index].getConnectionStatus()) {
            (*p_arduinos)[index].disconnect();
        } else (*p_arduinos)[index].connect();

        updateList();
    }
}
