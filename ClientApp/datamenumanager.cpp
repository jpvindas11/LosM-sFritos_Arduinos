#include "datamenumanager.h"

dataMenuManager::dataMenuManager()
{

}

void dataMenuManager::setPointers(std::vector<ArduinoSimulation> *arduinos,
    QListWidget* user_list) {
    this->p_arduinos = arduinos;
    this->user_list = user_list;
}

void dataMenuManager::updateList() {
    user_list->clear();

    for (ArduinoSimulation& arduino : *p_arduinos) {
        QString data = QString::fromStdString(arduino.sendData());

        QListWidgetItem *item = new QListWidgetItem(data, user_list);
        user_list->addItem(item);
    }
}

void dataMenuManager::setSelectedArduino(QListWidgetItem* user) {
    this->selectedArduino = user;
}

void dataMenuManager::turnOff() {
    if (!this->selectedArduino) return;

    int index = user_list->row(selectedArduino);
    if (index >= 0 && index < static_cast<int>(p_arduinos->size())) {
        if ((*p_arduinos)[index].getConnectionStatus()) {
            (*p_arduinos)[index].disconnect();
        } else (*p_arduinos)[index].connect();

        updateList();
    }
}

void dataMenuManager::getData() {
    for (ArduinoSimulation& arduino : *p_arduinos) {
        arduino.sendData();
    }
}
