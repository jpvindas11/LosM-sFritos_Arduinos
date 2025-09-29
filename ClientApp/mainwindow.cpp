#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menuwindow.h"
#include "FileSystem.hpp"
#include "AuthenticationServer.hpp"

MainWindow::MainWindow(AuthenticationServer* authServer, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
    , authServer(authServer)
{
    ui->setupUi(this);

    authServer->initialize();

    for (int i = 0; i < 3; ++i) {
        std::string userAU = "admin" + std::to_string(i);
        std::string passAU = "admin123";

        this->authServer->addUser(userAU, passAU, '1', '1');
    }
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

       MenuWindow* menu = new MenuWindow(this->authServer);
       userDataQt login;
       login.setData(user.toStdString(), pass.toStdString(), UR_OWNER);
       menu->setCurrentUser(login);
       menu->show();
       this->hide();

    } else {
        ui->login_info->setText("El usuario o contraseña son incorrectos");
    }

    ui->lineEdit_user->clear();
    ui->lineEdit_pass->clear();
}
