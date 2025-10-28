#ifndef DATAMENUMANAGER_H
#define DATAMENUMANAGER_H

#include <vector>
#include <QListWidget>
#include <QListWidgetItem>

class dataMenuManager
{
public:
    dataMenuManager();

    void setPointers(QListWidget* user_list);

    void updateList();

    void setSelectedArduino(QListWidgetItem* user);

    void turnOff();

    void getData();

private:
    QListWidget* user_list;
    QListWidgetItem* selectedArduino;
};

#endif // DATAMENUMANAGER_H
