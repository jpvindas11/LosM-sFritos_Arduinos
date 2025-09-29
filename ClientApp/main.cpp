#include "mainwindow.h"

#include <QApplication>
#include "FileSystem.hpp"

int main(int argc, char *argv[])
{
    FileSystem* fileSystem = new FileSystem();
    AuthenticationServer* authServer =  new AuthenticationServer(fileSystem);
    QApplication a(argc, argv);
    MainWindow mainWindow(authServer);
    mainWindow.show();
    return a.exec();
}
