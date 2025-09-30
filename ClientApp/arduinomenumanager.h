#ifndef ARDUINOMENUMANAGER_H
#define ARDUINOMENUMANAGER_H

#include <vector>
#include "arduinoSimulation.hpp"
#include <QListWidget>
#include <QListWidgetItem>

class arduinoMenuManager
{
public:
    arduinoMenuManager();

    void setPointers(std::vector<ArduinoSimulation>* arduinos, QListWidget* user_list);

    void updateList();

    void setSelectedArduino(QListWidgetItem* user);

    void turnOff();

private:
    std::vector<ArduinoSimulation>* p_arduinos;
    QListWidget* user_list;
    QListWidgetItem* selectedArduino;
};

#endif // ARDUINOMENUMANAGER_H
