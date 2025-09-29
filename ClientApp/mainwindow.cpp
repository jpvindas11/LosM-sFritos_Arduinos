#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menuwindow.h"
#include "FileSystem.hpp"
#include "AuthenticationServer.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
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
    FileSystem* fs = new FileSystem();
    AuthenticationServer* authServer = new AuthenticationServer(fs);
    authServer->initialize();
    std::string userAU = "admin";
    std::string passAU = "admin123";

    authServer->addUser(userAU, passAU);

    userDataQt login = this->autenticate.tryLogin(user, pass);

    std::string loginMessage = "LOGIN " + user.toStdString() + " " + pass.toStdString();
    authServer->setMessage(loginMessage);
    authServer->processMessage();
    authServer->sendMessage();

    if (authServer->status()) {
       ui->login_info->setText("Ingreso exitoso");

       MenuWindow* menu = new MenuWindow(this);
       menu->setCurrentUser(login);
       menu->show();
       this->hide();

    } else {
        ui->login_info->setText("El usuario o contraseña son incorrectos");
    }

    ui->lineEdit_user->clear();
    ui->lineEdit_pass->clear();
}
