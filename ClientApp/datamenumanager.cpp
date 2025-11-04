#include "datamenumanager.h"
#include <iostream>

dataMenuManager::dataMenuManager() : selectedSensor(nullptr)
{
}

void dataMenuManager::updateList(QListWidget* data_list,
                                 const std::vector<sensorRecentData>* sensorsData) {
    if (!data_list || !sensorsData) {
        std::cerr << "✗ updateList: NULL pointer received" << std::endl;
        return;
    }

    std::cout << "\n========== UPDATE LIST (Qt) ==========" << std::endl;
    std::cout << "Total sensors to display: " << sensorsData->size() << std::endl;

    data_list->clear();

    // Iterar sobre el vector de sensores
    for (size_t idx = 0; idx < sensorsData->size(); ++idx) {
        const auto& sensorData = (*sensorsData)[idx];

        std::cout << "\n[" << idx << "] Processing sensor:" << std::endl;
        std::cout << "  IP: '" << sensorData.ip << "'" << std::endl;
        std::cout << "  Type: '" << sensorData.sensorType << "'" << std::endl;
        std::cout << "  Data: '" << sensorData.data << "'" << std::endl;

        // Extraer solo el valor numérico de los datos
        // Los datos vienen en formato: "Distance:174cm,Timestamp:1762224331"
        QString dataStr = QString::fromStdString(sensorData.data);
        QString valueOnly;

        std::cout << "  Qt Data string: '" << dataStr.toStdString() << "'" << std::endl;

        // Buscar el primer ':' y extraer hasta la coma
        int colonPos = dataStr.indexOf(':');
        int commaPos = dataStr.indexOf(',');

        std::cout << "  Colon position: " << colonPos << std::endl;
        std::cout << "  Comma position: " << commaPos << std::endl;

        if (colonPos != -1 && commaPos != -1) {
            valueOnly = dataStr.mid(colonPos + 1, commaPos - colonPos - 1);
            std::cout << "  ✓ Extracted value: '" << valueOnly.toStdString() << "'" << std::endl;
        } else {
            // Si no se puede parsear, mostrar todo
            valueOnly = dataStr;
            std::cout << "  ✗ Could not extract value, using full data" << std::endl;
        }

        // Limpiar el tipo de sensor (quitar ':' si lo tiene)
        QString sensorType = QString::fromStdString(sensorData.sensorType);
        std::cout << "  Original type: '" << sensorType.toStdString() << "'" << std::endl;

        sensorType = sensorType.replace(":", "");
        std::cout << "  Cleaned type: '" << sensorType.toStdString() << "'" << std::endl;

        // Formato limpio: [TIPO] valor
        QString itemText = QString("[%1] %2")
                               .arg(sensorType)
                               .arg(valueOnly);

        std::cout << "  Final display text: '" << itemText.toStdString() << "'" << std::endl;

        QListWidgetItem *item = new QListWidgetItem(itemText, data_list);
        data_list->addItem(item);

        std::cout << "  ✓ Item added to list" << std::endl;
    }

    std::cout << "\n✓ List update complete (" << data_list->count() << " items)" << std::endl;
    std::cout << "======================================\n" << std::endl;
}

void dataMenuManager::setSelectedSensor(QListWidgetItem* sensor) {
    this->selectedSensor = sensor;
}

QListWidgetItem* dataMenuManager::getSelectedSensor() {
    return this->selectedSensor;
}

sensorRecentData* dataMenuManager::getSelectedSensorInfo(
    const std::vector<sensorRecentData>* sensorsData) {

    if (selectedSensor == nullptr || sensorsData == nullptr) {
        std::cerr << "✗ getSelectedSensorInfo: NULL pointer" << std::endl;
        return nullptr;
    }

    std::cout << "\n========== GET SELECTED SENSOR INFO ==========" << std::endl;

    // Obtener el texto del item
    QString itemText = selectedSensor->text();
    std::cout << "Selected item text: '" << itemText.toStdString() << "'" << std::endl;

    // Formato: "[TIPO] valor"

    // Extraer tipo de sensor
    int typeStart = itemText.indexOf("[") + 1;
    int typeEnd = itemText.indexOf("]");

    if (typeStart <= 0 || typeEnd <= 0 || typeEnd <= typeStart) {
        std::cerr << "✗ Could not parse item text" << std::endl;
        return nullptr;
    }

    QString typeStr = itemText.mid(typeStart, typeEnd - typeStart);
    std::string displayType = typeStr.toStdString();

    std::cout << "Extracted type: '" << displayType << "'" << std::endl;
    std::cout << "Searching in " << sensorsData->size() << " sensors..." << std::endl;

    // Buscar en el vector comparando el tipo (con o sin ':')
    for (auto& sensorData : *const_cast<std::vector<sensorRecentData>*>(sensorsData)) {
        QString storedType = QString::fromStdString(sensorData.sensorType);
        QString storedTypeCleaned = storedType.replace(":", "");

        std::cout << "  Comparing: '" << displayType << "' vs '"
                  << storedTypeCleaned.toStdString() << "'" << std::endl;

        if (storedTypeCleaned.toStdString() == displayType) {
            std::cout << "✓ Match found!" << std::endl;
            std::cout << "============================================\n" << std::endl;
            return &sensorData;
        }
    }

    std::cerr << "✗ No match found" << std::endl;
    std::cout << "============================================\n" << std::endl;
    return nullptr;
}
