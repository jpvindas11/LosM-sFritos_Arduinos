#include "menuwindow.h"
#include "mainwindow.h"
#include "ui_menuwindow.h"
#include "newuserdialog.h"
#include <QListWidgetItem>
#include "confirmdeleteuserdialog.h"

MenuWindow::MenuWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MenuWindow)
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

    this->hideMenuWidgets();
    this->hideFuctionsForRanks(user.getRank());
}

void MenuWindow::setSocket(Socket* socket) {
    this->socket = socket;
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
            this->ui->b_usuarios->move(0, 60);
            this->ui->b_arduinos->move(0, 90);
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

    // this->userMenu.setSelectedUser(nullptr);

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

    // this->userMenu.updateUserList();
}


void MenuWindow::on_b_arduinos_clicked()
{
    setActiveMenu(ui->b_arduinos, "Arduinos");

    // Scroll list
    this->ui->arduino_list->move(280,30);

    // Turn button
    this->ui->arduino_turn->move(170, 30);
    this->ui->b_consultar->move(-170, 90);

    // this->arduinoMenu.setSelectedArduino(nullptr);

    // this->arduinoMenu.updateList();
}

void MenuWindow::on_b_cerrarSesion_clicked()
{
    std::string IP = currentUser.getIP();
    int port = currentUser.getPort();

    std::cout << IP << std::endl;
    std::cout << port << std::endl;

    // Crear socket temporal para el logout
    Socket* tempSocket = nullptr;

    try {
        tempSocket = new Socket();

        // Crear y configurar el socket
        if (!tempSocket->create()) {
            delete tempSocket;
            QMessageBox::critical(this, "Error", "No se pudo crear el socket");
            return;
        }

        // Conectar al servidor
        if (!tempSocket->connectToServer(IP, port)) {
            delete tempSocket;
            QMessageBox::critical(this, "Error", "No se pudo conectar al servidor");
            return;
        }

        // Crear mensaje de logout usando Bitsery
        genMessage logout;
        logout.MID = static_cast<uint8_t>(MessageType::AUTH_LOGOUT);

        authLogout authLogoutReq;
        authLogoutReq.user = currentUser.getUser();
        logout.content = authLogoutReq;

        // Enviar solicitud de logout
        ssize_t sent = tempSocket->bSendData(tempSocket->getSocketFD(), logout);
        if (sent <= 0) {
            delete tempSocket;
            QMessageBox::critical(this, "Error", "No se pudo enviar la solicitud de logout");
            return;
        }

        // Recibir respuesta
        genMessage response;
        ssize_t received = tempSocket->bReceiveData(tempSocket->getSocketFD(), response);

        if (received <= 0) {
            delete tempSocket;
            QMessageBox::critical(this, "Error", "No se pudo recibir respuesta del servidor");
            return;
        }

        // Verificar tipo de respuesta
        if (response.MID == static_cast<uint8_t>(MessageType::OK_COMMON_MSG)) {
            try {
                delete tempSocket; // Limpiar socket antes de cambiar ventana

                MainWindow* login = new MainWindow();
                login->show();
                this->hide();

            } catch (const std::runtime_error& e) {
                QMessageBox::critical(this, "Error", "Error al procesar respuesta del servidor");
            }
        }
        else if (response.MID == static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG)) {
            errorCommonMsg errorMsg = getMessageContent<errorCommonMsg>(response);
            delete tempSocket;
            QMessageBox::critical(this, "Error", QString::fromStdString(errorMsg.message));
        }

    } catch (const std::exception& e) {
        if (tempSocket) delete tempSocket;
        QMessageBox::critical(this, "Error de conexión",
                              QString("No se pudo conectar al servidor: %1").arg(e.what()));
    }
}

void MenuWindow::on_user_list_itemClicked(QListWidgetItem *item)
{
    // this->userMenu.setSelectedUser(item);
    // userMenu.hideDeleteButton(ui->user_delete);
}


void MenuWindow::on_user_add_clicked()
{
    newUserDialog dialog(this);

    dialog.show();

    if (dialog.exec() == QDialog::Accepted) {
        std::string user = dialog.getUsername().toStdString();
        std::string pass = dialog.getPassword().toStdString();

        char rank = dialog.getRank();

        /*
         * Enviar MID para agregar un usuario
        */

        // this->userMenu.updateUserList();
    }
}


void MenuWindow::on_user_delete_clicked()
{
    /*
    confirmDeleteUserDialog dialog(this);

    QString message = QString("¿Estás seguro de querer eliminar a %1?")
                      .arg(QString::fromStdString(userDel->username));

    dialog.setUsername(message);

    dialog.show();

    if (dialog.exec() == QDialog::Accepted) {
        authServer->deleteUser(userDel->username);
    }
    */
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
    /*
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
*/
}

void MenuWindow::on_user_change_rank_clicked()
{
    /*
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
*/
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
    // this->dataMenu.updateList();
}


void MenuWindow::on_arduino_list_itemClicked(QListWidgetItem *item)
{
    // this->arduinoMenu.setSelectedArduino(item);
}


void MenuWindow::on_arduino_turn_clicked()
{
    this->arduinoMenu.turnOff();
}

void MenuWindow::on_arduino_consultar_Clicked(){
    // setActiveMenu("")
    // this->dataMenu.getData();
    // this->dataMenu.updateList();
}
