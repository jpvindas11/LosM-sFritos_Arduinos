#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menuwindow.h"
#include "autenticatorqt.h"
#include <string>

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
    std::string user = ui->lineEdit_user->text().toStdString();
    std::string pass = ui->lineEdit_pass->text().toStdString();

    userDataQt login = this->autenticate.tryLogin(user, pass);

    if (login.getRank() != -1) {
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
