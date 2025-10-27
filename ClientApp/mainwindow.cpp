#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menuwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
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

void MainWindow::on_pushButton_ip_clicked()
{
    QString ip = ui->lineEdit_IP->text();
    QString port = ui->lineEdit_PORT->text();

    // Iniciar el socket y enviar un mensaje a dicho IP y Puerto, para verificar que el servidor exista
    this->socket->create();

    if (!this->socket->connectToServer(ip.toStdString(), port.toInt())) {
        // No se encontró el servidor

        ui->lineEdit_IP->clear();
        ui->lineEdit_PORT->clear();
    }
}

