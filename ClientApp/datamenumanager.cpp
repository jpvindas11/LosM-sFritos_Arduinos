#include "datamenumanager.h"
#include <iostream>
#include <QFont>

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
    itemToSensorIndex.clear();

    // Agrupar sensores por IP
    std::map<std::string, std::vector<size_t>> sensorsByIP;

    for (size_t idx = 0; idx < sensorsData->size(); ++idx) {
        const auto& sensorData = (*sensorsData)[idx];
        sensorsByIP[sensorData.ip].push_back(idx);
    }

    std::cout << "Grouped into " << sensorsByIP.size() << " IPs" << std::endl;

    // Crear items agrupados por IP
    for (const auto& ipGroup : sensorsByIP) {
        const std::string& ip = ipGroup.first;
        const std::vector<size_t>& sensorIndices = ipGroup.second;

        std::cout << "\n[IP: " << ip << "] Processing " << sensorIndices.size() << " sensors" << std::endl;

        // Item principal (cabecera de IP)
        QString headerText = QString("🌐 Arduino: %1 (%2 sensores)")
                                 .arg(QString::fromStdString(ip))
                                 .arg(sensorIndices.size());

        QListWidgetItem* headerItem = new QListWidgetItem(headerText, data_list);
        QFont headerFont = headerItem->font();
        headerFont.setBold(true);
        headerFont.setPointSize(11);
        headerItem->setFont(headerFont);
        headerItem->setBackground(QColor(50, 60, 90, 180));
        headerItem->setForeground(QColor(0, 255, 255));

        // No es seleccionable (solo decorativo)
        headerItem->setFlags(headerItem->flags() & ~Qt::ItemIsSelectable);

        data_list->addItem(headerItem);

        // Agregar subsensores
        for (size_t idx : sensorIndices) {
            const auto& sensorData = (*sensorsData)[idx];

            std::cout << "  [" << idx << "] Processing sensor:" << std::endl;
            std::cout << "    Type: '" << sensorData.sensorType << "'" << std::endl;
            std::cout << "    Data: '" << sensorData.data << "'" << std::endl;

            // Extraer valor numérico
            QString dataStr = QString::fromStdString(sensorData.data);
            QString valueOnly;

            int colonPos = dataStr.indexOf(':');
            int commaPos = dataStr.indexOf(',');

            if (colonPos != -1 && commaPos != -1) {
                valueOnly = dataStr.mid(colonPos + 1, commaPos - colonPos - 1);
            } else {
                valueOnly = dataStr;
            }

            // Limpiar tipo de sensor
            QString sensorType = QString::fromStdString(sensorData.sensorType);
            sensorType = sensorType.replace(":", "");

            // Formato: "   📊 [TIPO] valor"
            QString itemText = QString("   📊 [%1] %2")
                                   .arg(sensorType)
                                   .arg(valueOnly);

            std::cout << "    Display text: '" << itemText.toStdString() << "'" << std::endl;

            QListWidgetItem* item = new QListWidgetItem(itemText, data_list);
            item->setBackground(QColor(40, 50, 80, 120));

            // Guardar referencia al índice del sensor
            itemToSensorIndex[item] = idx;

            data_list->addItem(item);

            std::cout << "    ✓ Item added to list" << std::endl;
        }

        // Separador visual
        QListWidgetItem* separator = new QListWidgetItem("", data_list);
        separator->setFlags(separator->flags() & ~Qt::ItemIsSelectable);
        separator->setSizeHint(QSize(0, 5));
        data_list->addItem(separator);
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

    // Buscar en el mapa de items
    auto it = itemToSensorIndex.find(selectedSensor);
    if (it != itemToSensorIndex.end()) {
        size_t idx = it->second;
        std::cout << "✓ Found sensor at index: " << idx << std::endl;
        std::cout << "============================================\n" << std::endl;
        return const_cast<sensorRecentData*>(&(*sensorsData)[idx]);
    }

    // Fallback: método antiguo de parsing
    QString itemText = selectedSensor->text();
    std::cout << "Selected item text: '" << itemText.toStdString() << "'" << std::endl;

    // Verificar si es un item de sensor (empieza con espacios y emoji)
    if (!itemText.startsWith("   ")) {
        std::cerr << "✗ Selected item is not a sensor (header or separator)" << std::endl;
        std::cout << "============================================\n" << std::endl;
        return nullptr;
    }

    // Formato: "   📊 [TIPO] valor"
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

    // Buscar en el vector comparando el tipo
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
