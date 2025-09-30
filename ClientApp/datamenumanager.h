#ifndef DATAMENUMANAGER_H
#define DATAMENUMANAGER_H

#include <vector>
#include "arduinoSimulation.hpp"
#include <QListWidget>
#include <QListWidgetItem>

class dataMenuManager
{
public:
    dataMenuManager();

    void setPointers(std::vector<ArduinoSimulation>* arduinos, QListWidget* user_list);

    void updateList();

    void setSelectedArduino(QListWidgetItem* user);

    void turnOff();

    void getData();

private:
    std::vector<ArduinoSimulation>* p_arduinos;
    QListWidget* user_list;
    QListWidgetItem* selectedArduino;
};

#endif // DATAMENUMANAGER_H
