#ifndef ARDUINOMENUMANAGER_H
#define ARDUINOMENUMANAGER_H

#include <vector>
#include <QListWidget>
#include <QListWidgetItem>

class arduinoMenuManager
{
public:
    arduinoMenuManager();

    void updateList();

    void setSelectedArduino(QListWidgetItem* user);

    void turnOff();

private:
    QListWidget* user_list;
    QListWidgetItem* selectedArduino;
};

#endif // ARDUINOMENUMANAGER_H
