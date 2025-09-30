#include "mainwindow.h"

#include <QApplication>
#include <iostream>
#include <ostream>
#include "FileSystem.hpp"
#include "Master.hpp"

int main(int argc, char *argv[])
{
    AuthenticationServer* authServer =  new AuthenticationServer();
    Master* masterServer = new Master();

    if (authServer->initialize() != 0) {
        delete masterServer;

        std::cerr << "Error al inicializar el servidor de autenticación" << std::endl;
        return -1;
    }

    std::string userAU = "admin";
    std::string passAU = "admin123";
    authServer->addUser(userAU, passAU, 'o', 'o');

    masterServer->generateArduinos(5);

    QApplication a(argc, argv);
    MainWindow mainWindow(authServer, masterServer);
    mainWindow.show();
    return a.exec();
}
