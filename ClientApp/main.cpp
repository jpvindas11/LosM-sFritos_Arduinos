#include "mainwindow.h"

#include <QApplication>
#include <iostream>
#include <ostream>
#include "FileSystem.hpp"

int main(int argc, char *argv[])
{
    AuthenticationServer* authServer =  new AuthenticationServer();

    if (authServer->initialize() != 0) {
        std::cerr << "Error al inicializar el servidor de autenticación" << std::endl;
        return -1;
    }

    QApplication a(argc, argv);
    MainWindow mainWindow(authServer);
    mainWindow.show();
    return a.exec();
}
