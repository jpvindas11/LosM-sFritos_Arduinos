#include "menuwindow.h"
#include "mainwindow.h"
#include "ui_menuwindow.h"
#include "newuserdialog.h"
#include <QListWidgetItem>
#include "confirmdeleteuserdialog.h"

MenuWindow::MenuWindow(AuthenticationServer* authServer, Master* masterServer,QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MenuWindow),
    authServer(authServer), masterServer(masterServer)
{
    ui->setupUi(this);

    this->userMenu.setPointers(this->ui->user_list, authServer->getUserMap(), &this->currentUser);
    this->arduinoMenu.setPointers(&masterServer->getAllArduinos(), this->ui->arduino_list);
    this->dataMenu.setPointers(&masterServer->getAllArduinos(), this->ui->data_list);
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

    this->hideMenuWidgets();
    this->hideFuctionsForRanks(user.getRank());
}

void MenuWindow::hideFuctionsForRanks(int rank) {
    // Primero ocultar todos los botones sensibles
    this->ui->b_usuarios->move(-100, -100);
    this->ui->b_arduinos->move(-100, -100);
    this->ui->b_nodos->move(-100, -100);
    this->ui->b_consulta->move(-100, -100);

    // Luego mostrar solo los permitidos según el rango
    switch(rank) {
        case UR_HARDWAREMANAGER:
            this->ui->b_arduinos->move(0, 30);
            break;
        case UR_USERMANAGER:
            this->ui->b_usuarios->move(0, 30);
            break;
        case UR_SOFTWAREMANAGER:
            this->ui->b_nodos->move(0, 30);
            break;
        case UR_CONSULTANT:
            this->ui->b_consulta->move(0, 30);
            break;
        case UR_OWNER:
            // OWNER tiene acceso a todo
            this->ui->b_consulta->move(0, 30);
            this->ui->b_usuarios->move(0, 60);    // Ajusta las posiciones
            this->ui->b_arduinos->move(0, 90);    // según tu diseño
            this->ui->b_nodos->move(0, 120);
            break;
        default:
            this->ui->b_consulta->move(0, 30);
            break;
    }
}

void MenuWindow::setActiveMenu(QPushButton *activeBtn, const QString &labelText)
{
    this->hideMenuWidgets();

    ui->main_label->setText(labelText);

    ui->b_usuarios->setEnabled(true);
    ui->b_arduinos->setEnabled(true);
    ui->b_consulta->setEnabled(true);
    ui->b_nodos->setEnabled(true);

    activeBtn->setEnabled(false);
}

void MenuWindow::on_b_usuarios_clicked()
{
    setActiveMenu(ui->b_usuarios, "Usuarios");

    this->userMenu.setSelectedUser(nullptr);

    // Scroll list
    this->ui->user_list->move(280,30);

    // Add button
    this->ui->user_add->move(170, 30);

    // Delete button
    this->ui->user_delete->move(170, 70);

    // Change label
    this->ui->label_cambios->move(170, 110);

    // Password button
    this->ui->user_change_pass->move(170, 130);

    // Rank button
    this->ui->user_change_rank->move(170, 170);

    this->userMenu.updateUserList();
}


void MenuWindow::on_b_arduinos_clicked()
{
    setActiveMenu(ui->b_arduinos, "Arduinos");

    // Scroll list
    this->ui->arduino_list->move(280,30);

    // Turn button
    this->ui->arduino_turn->move(170, 30);
    this->ui->b_consultar->move(-170, 90);

    this->arduinoMenu.setSelectedArduino(nullptr);

    this->arduinoMenu.updateList();
}


void MenuWindow::on_b_cerrarSesion_clicked()
{
    std::string logoutMessage = "LOGOUT " + currentUser.getUser();
    authServer->setMessage(logoutMessage);
    authServer->processMessage();
    authServer->sendMessage();
    MainWindow* login = new MainWindow(this->authServer, this->masterServer);
    login->show();
    this->hide();
}

void MenuWindow::on_user_list_itemClicked(QListWidgetItem *item)
{
    this->userMenu.setSelectedUser(item);
    userMenu.hideDeleteButton(ui->user_delete);
}


void MenuWindow::on_user_add_clicked()
{
    newUserDialog dialog(this);

    dialog.show();

    if (dialog.exec() == QDialog::Accepted) {
        std::string user = dialog.getUsername().toStdString();
        std::string pass = dialog.getPassword().toStdString();

        char rank = dialog.getRank();

        this->authServer->addUser(user, pass, rank, rank);

        this->userMenu.updateUserList();
    }
}


void MenuWindow::on_user_delete_clicked()
{
    AuthUser* userDel = this->userMenu.getSelectedAuthUser();

    // Has to have something selected
    if (!userDel) return;

    // Cannot delete self
    if (userDel->username == currentUser.getUser()) return;

    confirmDeleteUserDialog dialog(this);

    QString message = QString("¿Estás seguro de querer eliminar a %1?")
                      .arg(QString::fromStdString(userDel->username));

    dialog.setUsername(message);

    dialog.show();

    if (dialog.exec() == QDialog::Accepted) {
        // Funcion para borrar usuario aqui
    }
}

void MenuWindow::hideMenuWidgets() {
    // Reset widgets
    this->ui->user_list->move(-500,-100);
    this->ui->user_add->move(-500,-100);
    this->ui->user_delete->move(-500,-100);
    this->ui->label_cambios->move(-500,-100);
    this->ui->user_change_pass->move(-500,-100);
    this->ui->user_change_rank->move(-500,-100);
    this->ui->arduino_list->move(-500, -100);
    this->ui->arduino_turn->move(-500, -100);
    this->ui->b_consultar->move(-500, -100);
    this->ui->data_list->move(-1000, -100);
}

void MenuWindow::on_user_change_pass_clicked()
{
    AuthUser* userPass = this->userMenu.getSelectedAuthUser();

    // Has to have something selected
    if (!userPass) return;

    changePassDialog dialog(this);

    QString message = QString("Cambiar contraseña de %1")
                      .arg(QString::fromStdString(userPass->username));

    dialog.setUsername(message);

    dialog.show();

    if (dialog.exec() == QDialog::Accepted) {
        if (dialog.getFirstPass() == dialog.getSecondPass()) {
            // Funcion para cambiar contrasena de usuario
            authServer->changePassword(userPass->username , dialog.getFirstPass().toStdString());
        }
    }
}

void MenuWindow::on_user_change_rank_clicked()
{
    AuthUser* userRank = this->userMenu.getSelectedAuthUser();

    // Has to have something selected
    if (!userRank) return;

    // Cannot delete self
    if (userRank->username == currentUser.getUser()) return;

    setRankDialog dialog(this);

    QString message = QString("Cambiar permisos de %1")
                      .arg(QString::fromStdString(userRank->username));

    dialog.setUsername(message);

    dialog.show();

    if (dialog.exec() == QDialog::Accepted) {
        if (dialog.getRank() != '-') {
            // Funcion para cambiar rango del usuario
            authServer->changePermissions(userRank->username, userRank->rank, '1');
            userRank->rank = dialog.getRank();
        }
    }
}


void MenuWindow::on_b_nodos_clicked()
{
    setActiveMenu(ui->b_nodos, "Nodos Conectados (Sin Implementar)");
}


void MenuWindow::on_b_consulta_clicked()
{
    setActiveMenu(ui->b_consulta, "Información");

    // Scroll list
    this->ui->data_list->move(150,30);
    this->dataMenu.updateList();
}


void MenuWindow::on_arduino_list_itemClicked(QListWidgetItem *item)
{
    this->arduinoMenu.setSelectedArduino(item);
}


void MenuWindow::on_arduino_turn_clicked()
{
    this->arduinoMenu.turnOff();
}

void MenuWindow::on_arduino_consultar_Clicked(){
    // setActiveMenu("")
    this->dataMenu.getData();
    this->dataMenu.updateList();
}
