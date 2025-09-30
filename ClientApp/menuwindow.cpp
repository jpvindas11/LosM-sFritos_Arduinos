#include "menuwindow.h"
#include "mainwindow.h"
#include "ui_menuwindow.h"
#include "newuserdialog.h"
#include <QListWidgetItem>
#include "confirmdeleteuserdialog.h"

MenuWindow::MenuWindow(AuthenticationServer* authServer,QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MenuWindow),
    authServer(authServer)
{
    ui->setupUi(this);

    this->userMenu.setPointers(this->ui->user_list, authServer->getUserMap(), &this->currentUser);
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
    this->hideMenuWidgets();
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
    this->hideMenuWidgets();

    ui->main_label->setText(labelText);

    ui->b_usuarios->setEnabled(true);
    ui->b_arduinos->setEnabled(true);

    activeBtn->setEnabled(false);
}

void MenuWindow::on_b_usuarios_clicked()
{
    setActiveMenu(ui->b_usuarios, "Usuarios");

    // Scroll list
    this->ui->user_list->move(280,30);

    // Add button
    this->ui->user_add->move(170, 30);

    // Delete button
    this->ui->user_delete->move(170, 70);

    this->userMenu.updateUserList();
}


void MenuWindow::on_b_arduinos_clicked()
{
    setActiveMenu(ui->b_arduinos, "Arduinos");

}


void MenuWindow::on_b_cerrarSesion_clicked()
{
    std::string logoutMessage = "LOGOUT " + currentUser.getUser();
    authServer->setMessage(logoutMessage);
    authServer->processMessage();
    authServer->sendMessage();
    MainWindow* login = new MainWindow(this->authServer);
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

        this->authServer->addUser(user, pass, '1', '1');
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
}
