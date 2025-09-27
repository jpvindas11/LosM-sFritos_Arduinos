#include "menuwindow.h"
#include "mainwindow.h"
#include "ui_menuwindow.h"

MenuWindow::MenuWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MenuWindow)
{
    ui->setupUi(this);
}

MenuWindow::~MenuWindow()
{
    delete ui;
}

void MenuWindow::setCurrentUser(userDataQt user) {
    this->currentUser = user;

    // Saludar al usuario
    QString username = QString::fromStdString(user.getUser());

    ui->main_label->setText("Bienvenido, " + username);

    this->hideFuctionsForRanks(user.getRank());
}

void MenuWindow::hideFuctionsForRanks(int rank) {
    switch(rank) {
        case UR_HARDWAREMANAGER:
            this->ui->b_usuarios->move(-100, -100);
            break;
        case UR_USERMANAGER:
            this->ui->b_arduinos->move(-100, -100);
            break;
        case UR_OWNER:
        default:
            break;
    }
}

void MenuWindow::setActiveMenu(QPushButton *activeBtn, const QString &labelText)
{
    ui->main_label->setText(labelText);

    ui->b_usuarios->setEnabled(true);
    ui->b_arduinos->setEnabled(true);

    activeBtn->setEnabled(false);
}

void MenuWindow::on_b_usuarios_clicked()
{
    setActiveMenu(ui->b_usuarios, "Usuarios");
}


void MenuWindow::on_b_arduinos_clicked()
{
    setActiveMenu(ui->b_arduinos, "Arduinos");
}


void MenuWindow::on_b_cerrarSesion_clicked()
{
    MainWindow* login = new MainWindow(this);
    login->show();
    this->hide();
}
