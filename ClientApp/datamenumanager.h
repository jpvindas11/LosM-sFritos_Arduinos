#ifndef DATAMENUMANAGER_H
#define DATAMENUMANAGER_H

#include <vector>
#include <map>
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

    // Estructura para agrupar sensores por IP
    struct SensorGroup {
        std::string ip;
        std::vector<const sensorRecentData*> sensors;
    };

    // Mapa para tracking: item -> índice en sensorsData
    std::map<QListWidgetItem*, size_t> itemToSensorIndex;
};

#endif // DATAMENUMANAGER_H
