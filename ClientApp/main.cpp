#include "mainwindow.h"

#include <QApplication>
#include <iostream>
#include <ostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();
    return a.exec();
}
