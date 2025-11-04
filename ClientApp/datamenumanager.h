#ifndef DATAMENUMANAGER_H
#define DATAMENUMANAGER_H

#include <vector>
#include <QListWidget>
#include <QListWidgetItem>
#include "Messages.hpp"

class dataMenuManager
{
public:
    dataMenuManager();

    void updateList(QListWidget* data_list, const std::vector<sensorRecentData>* sensorsData);

    void setSelectedSensor(QListWidgetItem* sensor);

    QListWidgetItem* getSelectedSensor();

    sensorRecentData* getSelectedSensorInfo(const std::vector<sensorRecentData>* sensorsData);

private:
    QListWidgetItem* selectedSensor;
};

#endif // DATAMENUMANAGER_H
