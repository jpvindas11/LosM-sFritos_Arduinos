#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menuwindow.h"
#include "FileSystem.hpp"
#include "AuthenticationServer.hpp"


MainWindow::MainWindow(AuthenticationServer* authServer, Master* masterServer, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
    , authServer(authServer), masterServer(masterServer)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString user = ui->lineEdit_user->text();
    QString pass = ui->lineEdit_pass->text();

    if (this->autenticate.tryLogin(user.toStdString(), pass.toStdString(), authServer)) {
       ui->login_info->setText("Ingreso exitoso");

       MenuWindow* menu = new MenuWindow(this->authServer, this->masterServer);
       // Obtener el mapa de usuarios
       auto* userMap = authServer->getUserMap();

       // Buscar el usuario en el mapa
       auto it = userMap->find(user.toStdString());

       char userRank = 'c';
       if (it != userMap->end()) {
           userRank = it->second.rank;
           std::cout << "Usuario encontrado: " << it->second.username << std::endl;
           std::cout << "Rank (int): " << static_cast<int>(userRank) << std::endl;
           std::cout << "Rank (char): '" << userRank << "'" << std::endl;
           std::cout << "Rank (hex): 0x" << std::hex << static_cast<int>(userRank) << std::dec << std::endl;
       } else {
           std::cout << "Usuario no encontrado en el mapa" << std::endl;
       }

       userDataQt login;
       login.setData(user.toStdString(), pass.toStdString(), userRank);
       menu->setCurrentUser(login);
       menu->show();
       this->hide();

    } else {
        ui->login_info->setText("El usuario o contraseña son incorrectos");
    }

    ui->lineEdit_user->clear();
    ui->lineEdit_pass->clear();
}
