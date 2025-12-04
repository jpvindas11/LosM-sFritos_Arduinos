#include "menuwindow.h"
#include "mainwindow.h"
#include "ui_menuwindow.h"
#include "newuserdialog.h"
#include <QListWidgetItem>
#include "confirmdeleteuserdialog.h"
#include <QTextEdit>
#include <QFont>
#include <QLayout>

MenuWindow::MenuWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MenuWindow)
{
    ui->setupUi(this);

    sensorUpdateTimer = new QTimer(this);
    connect(sensorUpdateTimer, &QTimer::timeout, this, &MenuWindow::updateSensorDataAutomatically);

    hideMenuWidgets();
}

MenuWindow::~MenuWindow()
{
    if (sensorUpdateTimer) {
        sensorUpdateTimer->stop();
    }
    delete ui;

    std::string IP = currentUser.getIP();
    int port = currentUser.getPort();

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
    this->ui->b_usuarios->setVisible(false);
    this->ui->b_nodos->setVisible(false);
    this->ui->b_consulta->setVisible(false);
    this->ui->b_compus->setVisible(false);

    // Luego mostrar solo los permitidos según el rango
    switch(rank) {
    case UR_USERMANAGER:
        this->ui->b_usuarios->setVisible(true);
        break;
    case UR_HARDWAREMANAGER:
    case UR_SOFTWAREMANAGER:
        this->ui->b_compus->setVisible(true);
        this->ui->b_nodos->setVisible(true);
        break;
    case UR_CONSULTANT:
        this->ui->b_consulta->setVisible(true);
        break;
    case UR_OWNER:
        // OWNER tiene acceso a todo
        this->ui->b_consulta->setVisible(true);
        this->ui->b_usuarios->setVisible(true);
        this->ui->b_nodos->setVisible(true);
        this->ui->b_compus->setVisible(true);
        break;
    default:
        this->ui->b_consulta->setVisible(true);
        break;
    }
}

void MenuWindow::setActiveMenu(QPushButton *activeBtn, const QString &labelText, int pageIndex)
{
    // Detener timer si está activo
    if (sensorUpdateTimer && sensorUpdateTimer->isActive()) {
        sensorUpdateTimer->stop();
    }

    // ⭐ PRIMERO: Ocultar TODOS los widgets
    hideMenuWidgets();

    // Cambiar título
    ui->main_label->setText(labelText);

    // Habilitar todos los botones del menú
    ui->b_usuarios->setEnabled(true);
    ui->b_consulta->setEnabled(true);
    ui->b_nodos->setEnabled(true);
    ui->b_compus->setEnabled(true);

    // Deshabilitar el botón activo (muestra el glow)
    activeBtn->setEnabled(false);

    // Cambiar a la página correcta del StackedWidget
    ui->stackedWidget->setCurrentIndex(pageIndex);

    // ⭐ MOSTRAR widgets específicos según la página
    switch(pageIndex) {
    case 0: // Usuarios
        ui->user_list->setVisible(true);
        ui->user_add->setVisible(true);
        ui->user_delete->setVisible(true);
        ui->label_cambios->setVisible(true);
        ui->user_change_pass->setVisible(true);
        ui->user_change_rank->setVisible(true);
        std::cout << "✓ Usuarios widgets shown" << std::endl;
        break;

    case 1: // Nodos
        ui->arduino_list->setVisible(true);
        std::cout << "✓ Nodos widgets shown" << std::endl;
        break;

    case 2: // Consulta
        ui->data_list->setVisible(true);
        std::cout << "✓ Consulta widgets shown" << std::endl;
        break;

    case 3: // Computadoras
        ui->pc1_button->setVisible(true);
        ui->pc2_button->setVisible(true);
        ui->pc3_button->setVisible(true);
        ui->pc4_button->setVisible(true);
        std::cout << "✓ Computadoras widgets shown" << std::endl;
        break;
    }
}

void MenuWindow::askForUsers() {
    std::string IP = currentUser.getIP();
    int port = currentUser.getPort();

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

        genMessage usersMsg;
        usersMsg.MID = static_cast<uint8_t>(MessageType::AUTH_USER_REQUEST);
        usersMsg.content = authRequestUsers{};

        // Enviar solicitud de usuarios
        ssize_t sent = tempSocket->bSendData(tempSocket->getSocketFD(), usersMsg);
        if (sent <= 0) {
            delete tempSocket;
            QMessageBox::critical(this, "Error", "No se pudo enviar la solicitud de usuarios");
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
        if (response.MID == static_cast<uint8_t>(MessageType::AUTH_USER_RESPONSE)) {
            try {
                delete tempSocket; // Limpiar socket antes de cambiar ventana

                authRequestUsers usersResponse = getMessageContent<authRequestUsers>(response);

                this->users = usersResponse.users;

                this->userMenu.updateUserList(this->ui->user_list, &this->users, &this->currentUser);
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

void MenuWindow::askForSensorData() {
    std::cout << "\n========== ASK FOR SENSOR DATA ==========" << std::endl;

    std::string IP = currentUser.getIP();
    int port = currentUser.getPort();

    std::cout << "Connecting to: " << IP << ":" << port << std::endl;

    Socket* tempSocket = nullptr;

    try {
        tempSocket = new Socket();

        if (!tempSocket->create()) {
            delete tempSocket;
            QMessageBox::critical(this, "Error", "No se pudo crear el socket");
            return;
        }

        if (!tempSocket->connectToServer(IP, port)) {
            delete tempSocket;
            QMessageBox::critical(this, "Error", "No se pudo conectar al servidor");
            return;
        }

        std::cout << "✓ Connected to server" << std::endl;

        genMessage sensorsMsg;
        sensorsMsg.MID = static_cast<uint8_t>(MessageType::SEN_RECENT_DATA_REQ);
        GenNumReq request;
        request.id_token = 0;
        sensorsMsg.content = request;

        std::cout << "Sending SEN_RECENT_DATA_REQ message..." << std::endl;

        ssize_t sent = tempSocket->bSendData(tempSocket->getSocketFD(), sensorsMsg);
        if (sent <= 0) {
            delete tempSocket;
            QMessageBox::critical(this, "Error", "No se pudo enviar la solicitud");
            return;
        }

        std::cout << "✓ Request sent (" << sent << " bytes)" << std::endl;
        std::cout << "Waiting for response..." << std::endl;

        genMessage response;
        ssize_t received = tempSocket->bReceiveData(tempSocket->getSocketFD(), response);

        if (received <= 0) {
            delete tempSocket;
            QMessageBox::critical(this, "Error", "No se pudo recibir respuesta del servidor");
            return;
        }

        std::cout << "✓ Response received (" << received << " bytes)" << std::endl;
        std::cout << "Response MID: " << static_cast<int>(response.MID) << std::endl;

        if (response.MID == static_cast<uint8_t>(MessageType::SEN_RECENT_DATA_RES)) {
            try {
                delete tempSocket;

                senRecentDataRes sensorsResponse = getMessageContent<senRecentDataRes>(response);

                std::cout << "\n========== RECEIVED SENSOR DATA ==========" << std::endl;
                std::cout << "Total sensors received: " << sensorsResponse.recentData.size() << std::endl;

                for (size_t i = 0; i < sensorsResponse.recentData.size(); ++i) {
                    const auto& sensor = sensorsResponse.recentData[i];

                    std::cout << "\n[" << i << "] Sensor details:" << std::endl;
                    std::cout << "  IP: '" << sensor.ip << "' (length: " << sensor.ip.length() << ")" << std::endl;
                    std::cout << "  Type: '" << sensor.sensorType << "' (length: " << sensor.sensorType.length() << ")" << std::endl;
                    std::cout << "  Data: '" << sensor.data << "' (length: " << sensor.data.length() << ")" << std::endl;
                    std::cout << "  LastModified: " << sensor.lastModified << std::endl;

                    // Mostrar en hexadecimal
                    std::cout << "  Type (HEX): ";
                    for (char c : sensor.sensorType) {
                        printf("%02X ", (unsigned char)c);
                    }
                    std::cout << std::endl;

                    std::cout << "  Data (HEX): ";
                    for (size_t j = 0; j < std::min(sensor.data.length(), size_t(50)); ++j) {
                        printf("%02X ", (unsigned char)sensor.data[j]);
                    }
                    std::cout << std::endl;
                }

                this->sensorsData = sensorsResponse.recentData;

                std::cout << "\nCalling updateList..." << std::endl;
                this->dataMenu.updateList(this->ui->data_list, &this->sensorsData);

                std::cout << "✓ List updated successfully" << std::endl;
                std::cout << "==========================================\n" << std::endl;

            } catch (const std::runtime_error& e) {
                std::cerr << "✗ Error processing response: " << e.what() << std::endl;
                QMessageBox::critical(this, "Error", "Error al procesar respuesta del servidor");
            }
        }
        else if (response.MID == static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG)) {
            errorCommonMsg errorMsg = getMessageContent<errorCommonMsg>(response);
            delete tempSocket;
            std::cerr << "✗ Server error: " << errorMsg.message << std::endl;
            QMessageBox::critical(this, "Error", QString::fromStdString(errorMsg.message));
        }

    } catch (const std::exception& e) {
        if (tempSocket) delete tempSocket;
        std::cerr << "✗ Exception: " << e.what() << std::endl;
        QMessageBox::critical(this, "Error de conexión",
                              QString("No se pudo conectar al servidor: %1").arg(e.what()));
    }
}

void MenuWindow::on_b_usuarios_clicked()
{
    setActiveMenu(ui->b_usuarios, "👥 Usuarios", 0);

    // Cambiar tema a morado
    ui->user_list->setStyleSheet(
        "QListWidget { border: 2px solid #7b2ff7; }"
        "QListWidget::item:selected { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "                              stop:0 #7b2ff7, stop:1 #9351ff); "
        "}"
        );

    askForUsers();
}

void MenuWindow::on_b_consulta_clicked()
{
    setActiveMenu(ui->b_consulta, "📊 Información de Sensores", 2);

    // Cambiar tema a naranja/amarillo
    ui->data_list->setStyleSheet(
        "QListWidget { border: 2px solid #ff9500; }"
        "QListWidget::item:selected { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "                              stop:0 #ff6600, stop:1 #ffbb00); "
        "}"
        );

    askForSensorData();
    sensorUpdateTimer->start(3000);
}

void MenuWindow::on_b_nodos_clicked()
{
    setActiveMenu(ui->b_nodos, "🌐 Nodos Conectados", 1);

    // Cambiar tema a verde
    ui->arduino_list->setStyleSheet(
        "QListWidget { border: 2px solid #39ff14; }"
        "QListWidget::item:selected { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "                              stop:0 #00ff00, stop:1 #39ff14); "
        "}"
        );

    askForServerStatus();
}

void MenuWindow::on_b_compus_clicked()
{
    setActiveMenu(ui->b_compus, "💻 Computadoras", 3);

    // Cambiar tema de botones PC a rojo/rosa
    QString pcStyle =
        "QPushButton { border: 3px solid #ff006e; }"
        "QPushButton:hover { "
        "  border: 3px solid #ff1a7a; "
        "  box-shadow: 0 0 30px rgba(255, 0, 110, 0.8); "
        "}";

    ui->pc1_button->setStyleSheet(pcStyle);
    ui->pc2_button->setStyleSheet(pcStyle);
    ui->pc3_button->setStyleSheet(pcStyle);
    ui->pc4_button->setStyleSheet(pcStyle);
}

void MenuWindow::on_b_cerrarSesion_clicked()
{
    std::string IP = currentUser.getIP();
    int port = currentUser.getPort();

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
    this->userMenu.setSelectedUser(item);
    this->userMenu.hideDeleteButton(ui->user_delete, &this->currentUser);
}

void MenuWindow::on_user_list_itemDoubleClicked(QListWidgetItem *item)
{
    this->userMenu.setSelectedUser(item);

    // Obtener información del usuario seleccionado
    UserInfo* selectedUser = this->userMenu.getSelectedUserInfo(&this->users);

    if (selectedUser) {
        // Crear mensaje box con opciones
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Usuario: " + QString::fromStdString(selectedUser->user));

        QString info = QString(
                           "╔═══════════════════════════════════════╗\n"
                           "INFORMACIÓN DEL USUARIO\n"
                           "╚═══════════════════════════════════════╝\n\n"
                           "Usuario:        %1\n"
                           "Rango:          %2\n"
                           "Estado:         %3\n\n"
                           "¿Qué deseas hacer?"
                           ).arg(QString::fromStdString(selectedUser->user))
                           .arg(getRankName(selectedUser->rank))
                           .arg(selectedUser->isConnected ? "Conectado" : "Desconectado");

        msgBox.setText(info);
        msgBox.setIcon(QMessageBox::Question);

        // Agregar botones personalizados
        QPushButton* logsButton = msgBox.addButton("Ver Logs", QMessageBox::ActionRole);
        QPushButton* closeButton = msgBox.addButton("Cerrar", QMessageBox::RejectRole);

        msgBox.exec();

        // Verificar qué botón se presionó
        if (msgBox.clickedButton() == logsButton) {
            // Solicitar y mostrar logs
            askForUserLogs(selectedUser->user);
        }
    }
}


void MenuWindow::on_user_add_clicked()
{
    newUserDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        std::string user = dialog.getUsername().toStdString();
        std::string pass = dialog.getPassword().toStdString();
        char rank = dialog.getRank();
        std::string IP = currentUser.getIP();
        int port = currentUser.getPort();
        std::cout << IP << std::endl;
        std::cout << port << std::endl;

        // Cancel if non-owner tried to create an owner
        if (currentUser.getRank() != UR_OWNER && dialog.getRank() == USER_OWNER) {
            QMessageBox::critical(this, "Error", "Solo dueños pueden crear otros DUEÑOS");
            return;
        }

        // Crear socket temporal para la creación de usuario
        Socket* tempSocket = nullptr;
        try {
            tempSocket = new Socket();

            // Crear y configurar el socket
            if (!tempSocket->create()) {
                delete tempSocket;
                QMessageBox::critical(this, "Error", "No se pudo crear el socket");
                return;
            }

            std::cout << "Socket creado" << std::endl;

            // Conectar al servidor
            if (!tempSocket->connectToServer(IP, port)) {
                delete tempSocket;
                QMessageBox::critical(this, "Error", "No se pudo conectar al servidor");
                return;
            }

            std::cout << "Conectado al server" << std::endl;

            // Crear mensaje de creación de usuario usando Bitsery
            genMessage msg;
            msg.MID = static_cast<uint8_t>(MessageType::AUTH_USER_CREATE);
            authCreateUser authCreate;
            authCreate.newUser = user;
            authCreate.pass = pass;
            authCreate.rank = rank;
            msg.content = authCreate;

            // Enviar solicitud de creación
            ssize_t sent = tempSocket->bSendData(tempSocket->getSocketFD(), msg);
            if (sent <= 0) {
                delete tempSocket;
                QMessageBox::critical(this, "Error", "No se pudo enviar la solicitud de creación");
                return;
            }

            std::cout << "Enviado a master" << std::endl;

            // Recibir respuesta
            genMessage response;
            ssize_t received = tempSocket->bReceiveData(tempSocket->getSocketFD(), response);
            if (received <= 0) {
                delete tempSocket;
                QMessageBox::critical(this, "Error", "No se pudo recibir respuesta del servidor");
                return;
            }

            std::cout << "Respuesta recibida" << std::endl;

            // Limpiar socket
            delete tempSocket;
            tempSocket = nullptr;

            QMessageBox::information(this, "INFO", "Usuario creado correctamente");

        } catch (const std::exception& e) {
            if (tempSocket) {
                delete tempSocket;
                tempSocket = nullptr;
            }
            QMessageBox::critical(this, "Error de conexión",
                                  QString("No se pudo conectar al servidor: %1").arg(e.what()));
        }

        this->askForUsers();

    }
}


void MenuWindow::on_user_delete_clicked()
{
    UserInfo* userDel = this->userMenu.getSelectedUserInfo(&this->users);

    if (!userDel) {
        QMessageBox::warning(this, "Aviso", "No hay usuario seleccionado");
        return;
    }

    if (userDel->user == currentUser.getUser()) {
        QMessageBox::warning(this, "Aviso", "No puedes eliminarte a ti mismo");
        return;
    }

    confirmDeleteUserDialog dialog(this);

    QString message = QString("¿Estás seguro de querer eliminar a %1?").arg(QString::fromStdString(userDel->user));

    dialog.setUsername(message);

    if (dialog.exec() == QDialog::Accepted) {
        // Aquí enviarías el mensaje de eliminación al servidor
        std::string IP = currentUser.getIP();
        int port = currentUser.getPort();

        // Cancel if non-owner tried to create an owner
        if (currentUser.getRank() != UR_OWNER && userDel->rank == USER_OWNER) {
            QMessageBox::critical(this, "Error", "Solo dueños pueden eliminar a otro DUEÑO");
            return;
        }

        // Cancel if non-owner tried to create an owner
        if (userDel->isConnected) {
            QMessageBox::critical(this, "Error", "No puedes eliminar a un usuario en linea");
            return;
        }

        Socket* tempSocket = nullptr;
        try {
            tempSocket = new Socket();

            if (!tempSocket->create() || !tempSocket->connectToServer(IP, port)) {
                delete tempSocket;
                QMessageBox::critical(this, "Error", "No se pudo conectar al servidor");
                return;
            }

            genMessage deleteMsg;
            deleteMsg.MID = static_cast<uint8_t>(MessageType::AUTH_USER_DELETE);
            authDeleteUser delUser;
            delUser.deleteUser = userDel->user;
            deleteMsg.content = delUser;

            ssize_t sent = tempSocket->bSendData(tempSocket->getSocketFD(), deleteMsg);
            if (sent <= 0) {
                delete tempSocket;
                QMessageBox::critical(this, "Error", "No se pudo enviar solicitud");
                return;
            }

            genMessage response;
            ssize_t received = tempSocket->bReceiveData(tempSocket->getSocketFD(), response);
            delete tempSocket;

            if (received > 0 && response.MID == static_cast<uint8_t>(MessageType::OK_COMMON_MSG)) {
                QMessageBox::information(this, "Aviso", "Usuario eliminado correctamente");

                // Actualizar lista
                this->askForUsers();
                this->userMenu.updateUserList(this->ui->user_list, &this->users, &this->currentUser);
            } else {
                QMessageBox::critical(this, "Error", "No se pudo eliminar el usuario");
            }

        } catch (const std::exception& e) {
            if (tempSocket) delete tempSocket;
            QMessageBox::critical(this, "Error", e.what());
        }
    }
}

void MenuWindow::hideMenuWidgets() {
    // Ocultar widgets del menú de usuarios
    this->ui->user_list->setVisible(false);
    this->ui->user_add->setVisible(false);
    this->ui->user_delete->setVisible(false);
    this->ui->label_cambios->setVisible(false);
    this->ui->user_change_pass->setVisible(false);
    this->ui->user_change_rank->setVisible(false);

    // Ocultar widgets del menú de nodos
    this->ui->arduino_list->setVisible(false);

    // Ocultar widgets del menú de consulta
    this->ui->data_list->setVisible(false);

    // Ocultar widgets del menú de computadoras
    this->ui->pc1_button->setVisible(false);
    this->ui->pc2_button->setVisible(false);
    this->ui->pc3_button->setVisible(false);
    this->ui->pc4_button->setVisible(false);

    // Detener timer si está activo
    if (sensorUpdateTimer && sensorUpdateTimer->isActive()) {
        sensorUpdateTimer->stop();
    }

    std::cout << "✓ All menu widgets hidden" << std::endl;
}

void MenuWindow::on_user_change_pass_clicked()
{
    UserInfo* userPass = this->userMenu.getSelectedUserInfo(&this->users);

    if (!userPass) {
        QMessageBox::warning(this, "Advertencia", "No hay usuario seleccionado");
        return;
    }

    changePassDialog dialog(this);

    QString message = QString("Cambiar contraseña de %1").arg(QString::fromStdString(userPass->user));

    dialog.setUsername(message);

    if (dialog.exec() == QDialog::Accepted) {
        if (dialog.getFirstPass() != dialog.getSecondPass()) {
            QMessageBox::warning(this, "Advertencia", "Ambas contraseñas deben ser iguales");
            return;
        }
        else if (dialog.getFirstPass().size() <= 4) {
            QMessageBox::warning(this, "Advertencia", "La nueva contraseña es demasiado corta");
            return;
        }

        std::string IP = currentUser.getIP();
        int port = currentUser.getPort();

        Socket* tempSocket = nullptr;
        try {
            tempSocket = new Socket();

            if (!tempSocket->create() || !tempSocket->connectToServer(IP, port)) {
                delete tempSocket;
                QMessageBox::critical(this, "Error", "No se pudo conectar al servidor");
                return;
            }

            genMessage changeMsg;
            changeMsg.MID = static_cast<uint8_t>(MessageType::AUTH_USER_MODIFY_PASS);
            authModifyUserPass changeUser;
            changeUser.user = userPass->user;
            changeUser.newPassword = dialog.getFirstPass().toStdString();
            changeMsg.content = changeUser;

            ssize_t sent = tempSocket->bSendData(tempSocket->getSocketFD(), changeMsg);
            if (sent <= 0) {
                delete tempSocket;
                QMessageBox::critical(this, "Error", "No se pudo enviar solicitud");
                return;
            }

            genMessage response;
            ssize_t received = tempSocket->bReceiveData(tempSocket->getSocketFD(), response);
            delete tempSocket;

            if (received > 0 && response.MID == static_cast<uint8_t>(MessageType::OK_COMMON_MSG)) {
                QMessageBox::information(this, "Aviso", "Usuario modificado correctamente");

                // Actualizar lista
                this->askForUsers();
                this->userMenu.updateUserList(this->ui->user_list, &this->users, &this->currentUser);
            } else {
                QMessageBox::critical(this, "Error", "No se pudo modificar el usuario");
            }

        } catch (const std::exception& e) {
            if (tempSocket) delete tempSocket;
            QMessageBox::critical(this, "Error", e.what());
        }
    }
}

void MenuWindow::on_user_change_rank_clicked()
{
    UserInfo* userRank = this->userMenu.getSelectedUserInfo(&this->users);

    if (!userRank) {
        QMessageBox::warning(this, "Advertencia", "No hay usuario seleccionado");
        return;
    }

    setRankDialog dialog(this);

    QString message = QString("Cambiar contraseña de %1").arg(QString::fromStdString(userRank->user));

    dialog.setUsername(message);

    if (dialog.exec() == QDialog::Accepted) {
        std::string IP = currentUser.getIP();
        int port = currentUser.getPort();

        // Cancel if non-owner tried to create an owner
        if (currentUser.getRank() != UR_OWNER && dialog.getRank() == USER_OWNER) {
            QMessageBox::critical(this, "Error", "Solo dueños pueden asignar el rango DUEÑO");
            return;
        }

        Socket* tempSocket = nullptr;
        try {
            tempSocket = new Socket();

            if (!tempSocket->create() || !tempSocket->connectToServer(IP, port)) {
                delete tempSocket;
                QMessageBox::critical(this, "Error", "No se pudo conectar al servidor");
                return;
            }

            genMessage changeMsg;
            changeMsg.MID = static_cast<uint8_t>(MessageType::AUTH_USER_MODIFY_RANK);
            authModifyUserRank changeUser;
            changeUser.user = userRank->user;
            changeUser.rank = dialog.getRank();

            changeMsg.content = changeUser;

            ssize_t sent = tempSocket->bSendData(tempSocket->getSocketFD(), changeMsg);
            if (sent <= 0) {
                delete tempSocket;
                QMessageBox::critical(this, "Error", "No se pudo enviar solicitud");
                return;
            }

            genMessage response;
            ssize_t received = tempSocket->bReceiveData(tempSocket->getSocketFD(), response);
            delete tempSocket;

            if (received > 0 && response.MID == static_cast<uint8_t>(MessageType::OK_COMMON_MSG)) {
                QMessageBox::information(this, "Aviso", "Usuario modificado correctamente");

                // Actualizar lista
                this->askForUsers();
                this->userMenu.updateUserList(this->ui->user_list, &this->users, &this->currentUser);
            } else {
                QMessageBox::critical(this, "Error", "No se pudo modificar el usuario");
            }

        } catch (const std::exception& e) {
            if (tempSocket) delete tempSocket;
            QMessageBox::critical(this, "Error", e.what());
        }
    }
}

void MenuWindow::on_data_list_itemClicked(QListWidgetItem *item)
{
    // Verificar si es un header o separador (no seleccionable)
    if (!(item->flags() & Qt::ItemIsSelectable)) {
        std::cout << "✗ Clicked on non-selectable item (header/separator)" << std::endl;
        return;
    }

    this->dataMenu.setSelectedSensor(item);

    sensorRecentData* sensor = this->dataMenu.getSelectedSensorInfo(&this->sensorsData);

    if (sensor) {
        time_t currentTime = time(nullptr);
        uint32_t secondsAgo = currentTime - sensor->lastModified;

        QString timeStr;
        if (secondsAgo < 60) {
            timeStr = QString("%1 segundo(s)").arg(secondsAgo);
        } else if (secondsAgo < 3600) {
            timeStr = QString("%1 minuto(s)").arg(secondsAgo / 60);
        } else {
            timeStr = QString("%1 hora(s)").arg(secondsAgo / 3600);
        }

        QString dataStr = QString::fromStdString(sensor->data);
        QString parsedValue;

        int colonPos = dataStr.indexOf(':');
        int commaPos = dataStr.indexOf(',');
        if (colonPos != -1 && commaPos != -1) {
            parsedValue = dataStr.mid(colonPos + 1, commaPos - colonPos - 1);
        } else {
            parsedValue = dataStr;
        }

        QString sensorType = QString::fromStdString(sensor->sensorType);
        sensorType = sensorType.replace(":", "");

        QString info = QString(
                           "╔═══════════════════════════════════════╗\n"
                           "    INFORMACIÓN DEL SENSOR\n"
                           "╚═══════════════════════════════════════╝\n\n"
                           "Tipo:                %1\n"
                           "IP:                  %2\n"
                           "Valor actual:        %3\n"
                           "Última actualización: %4 atrás\n\n"
                           "¿Qué deseas hacer?"
                           ).arg(sensorType)
                           .arg(QString::fromStdString(sensor->ip))
                           .arg(parsedValue)
                           .arg(timeStr);

        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Sensor: " + sensorType);
        msgBox.setText(info);
        msgBox.setIcon(QMessageBox::Question);

        QPushButton* logsButton = msgBox.addButton("Ver Historial", QMessageBox::ActionRole);
        QPushButton* closeButton = msgBox.addButton("Cerrar", QMessageBox::RejectRole);

        msgBox.exec();

        if (msgBox.clickedButton() == logsButton) {
            askForSensorLogs(sensor->ip, sensor->sensorType);
        }
    } else {
        QMessageBox::warning(this, "Error", "No se pudo obtener información del sensor");
    }
}

void MenuWindow::askForUserLogs(const std::string& username) {
    std::cout << "\n========== ASK FOR USER LOGS ==========" << std::endl;
    std::cout << "Requesting logs for user: " << username << std::endl;

    std::string IP = currentUser.getIP(); // IP del servidor de logs
    int port = currentUser.getPort(); // Puerto del servidor de logs (PORT_MASTER_LOGS)

    Socket* tempSocket = nullptr;

    try {
        tempSocket = new Socket();

        if (!tempSocket->create()) {
            delete tempSocket;
            QMessageBox::critical(this, "Error", "No se pudo crear el socket");
            return;
        }

        if (!tempSocket->connectToServer(IP, port)) {
            delete tempSocket;
            QMessageBox::critical(this, "Error",
                                  "No se pudo conectar al servidor de logs en " +
                                      QString::fromStdString(IP) + ":" + QString::number(port));
            return;
        }

        std::cout << "✓ Connected to logs server" << std::endl;

        // Crear mensaje de solicitud
        genMessage logsRequest;
        logsRequest.MID = static_cast<uint8_t>(MessageType::LOG_USER_REQUEST);

        userLogRequestCommon request;
        request.id_token = 0; // No necesitamos token para esto
        request.userName = username;

        logsRequest.content = request;

        std::cout << "Sending LOG_USER_REQUEST for: " << username << std::endl;

        ssize_t sent = tempSocket->bSendData(tempSocket->getSocketFD(), logsRequest);
        if (sent <= 0) {
            delete tempSocket;
            QMessageBox::critical(this, "Error", "No se pudo enviar la solicitud de logs");
            return;
        }

        std::cout << "✓ Request sent (" << sent << " bytes)" << std::endl;

        // Recibir todas las páginas de logs
        QString allLogs;
        bool receivingPages = true;
        int pagesReceived = 0;
        uint32_t totalPages = 0;

        while (receivingPages) {
            genMessage response;
            ssize_t received = tempSocket->bReceiveData(tempSocket->getSocketFD(), response);

            if (received <= 0) {
                std::cerr << "✗ Error receiving response or connection closed" << std::endl;
                break;
            }

            std::cout << "Response MID: " << static_cast<int>(response.MID) << std::endl;

            if (response.MID == static_cast<uint8_t>(MessageType::LOG_USER_RESP)) {
                try {
                    userLogResp logsResponse = getMessageContent<userLogResp>(response);

                    totalPages = logsResponse.totalPages;
                    pagesReceived++;

                    std::cout << "Received page " << logsResponse.page
                              << " of " << logsResponse.totalPages << std::endl;

                    // Procesar primer bloque
                    if (!logsResponse.firstBlock.empty()) {
                        std::string block1 = logsResponse.firstBlock;
                        // Eliminar padding de nulls
                        size_t nullPos = block1.find('\0');
                        if (nullPos != std::string::npos) {
                            block1 = block1.substr(0, nullPos);
                        }
                        allLogs += QString::fromStdString(block1);
                    }

                    // Procesar segundo bloque
                    if (!logsResponse.secondBlock.empty()) {
                        std::string block2 = logsResponse.secondBlock;
                        size_t nullPos = block2.find('\0');
                        if (nullPos != std::string::npos) {
                            block2 = block2.substr(0, nullPos);
                        }
                        allLogs += QString::fromStdString(block2);
                    }

                    // Verificar si es la última página
                    if (logsResponse.page >= logsResponse.totalPages) {
                        std::cout << "✓ All pages received (" << pagesReceived
                                  << " pages)" << std::endl;
                        receivingPages = false;
                    }

                } catch (const std::runtime_error& e) {
                    std::cerr << "✗ Error processing response: " << e.what() << std::endl;
                    QMessageBox::critical(this, "Error",
                                          "Error al procesar respuesta del servidor de logs");
                    break;
                }
            }
            else if (response.MID == static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG)) {
                errorCommonMsg errorMsg = getMessageContent<errorCommonMsg>(response);
                std::cerr << "✗ Server error: " << errorMsg.message << std::endl;

                delete tempSocket;

                // Mostrar mensaje informativo si no hay logs
                if (std::string(errorMsg.message).find("not found") != std::string::npos ||
                    std::string(errorMsg.message).find("Empty") != std::string::npos) {
                    QMessageBox::information(this, "Sin logs",
                                             QString("El usuario '%1' no tiene logs registrados todavía.")
                                                 .arg(QString::fromStdString(username)));
                } else {
                    QMessageBox::critical(this, "Error",
                                          QString::fromStdString(errorMsg.message));
                }
                return;
            }
        }

        delete tempSocket;

        // Mostrar logs en un popup
        if (!allLogs.isEmpty()) {
            // Contar número de líneas
            int logCount = allLogs.count('\n');

            // Crear mensaje formateado
            QString formattedLogs = QString(
                                        "═══════════════════════════════════════\n"
                                        "       LOGS DE USUARIO\n"
                                        "═══════════════════════════════════════\n\n"
                                        "Usuario:        %1\n"
                                        "Total de logs:  %2\n"
                                        "Páginas:        %3\n\n"
                                        "═══════════════════════════════════════\n"
                                        "HISTORIAL DE ACTIVIDAD:\n"
                                        "═══════════════════════════════════════\n\n%4"
                                        ).arg(QString::fromStdString(username))
                                        .arg(logCount)
                                        .arg(totalPages)
                                        .arg(allLogs);

            // Crear QMessageBox personalizado con scroll
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Logs de Usuario - " + QString::fromStdString(username));
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setStandardButtons(QMessageBox::Ok);

            // Usar QTextEdit para permitir scroll si hay muchos logs
            QTextEdit* textEdit = new QTextEdit();
            textEdit->setReadOnly(true);
            textEdit->setPlainText(formattedLogs);
            textEdit->setMinimumSize(600, 400);
            textEdit->setFont(QFont("Courier New", 9)); // Fuente monoespaciada

            msgBox.layout()->addWidget(textEdit);
            msgBox.exec();

        } else {
            QMessageBox::information(this, "Sin logs",
                                     QString("El usuario '%1' no tiene logs registrados.")
                                         .arg(QString::fromStdString(username)));
        }

        std::cout << "==========================================\n" << std::endl;

    } catch (const std::exception& e) {
        if (tempSocket) delete tempSocket;
        std::cerr << "✗ Exception: " << e.what() << std::endl;
        QMessageBox::critical(this, "Error de conexión",
                              QString("No se pudo conectar al servidor de logs: %1").arg(e.what()));
    }
}

void MenuWindow::updateSensorDataAutomatically() {
    askForSensorData();
}


void MenuWindow::askForSensorLogs(const std::string& sensorIP, const std::string& sensorType) {
    std::cout << "\n========== ASK FOR SENSOR LOGS ==========" << std::endl;
    std::cout << "Requesting logs for sensor:" << std::endl;
    std::cout << "  IP: " << sensorIP << std::endl;
    std::cout << "  Type: " << sensorType << std::endl;

    std::string IP = currentUser.getIP();
    int port = currentUser.getPort();

    std::cout << "Connecting to Master: " << IP << ":" << port << std::endl;

    // ========================================
    // PASO 1: Obtener lista de archivos
    // ========================================
    Socket* tempSocket = new Socket();

    if (!tempSocket->create()) {
        delete tempSocket;
        QMessageBox::critical(this, "Error", "No se pudo crear el socket");
        return;
    }

    if (!tempSocket->connectToServer(IP, port)) {
        delete tempSocket;
        QMessageBox::critical(this, "Error", "No se pudo conectar al servidor Master");
        return;
    }

    std::cout << "✓ Connected to Master server" << std::endl;

    genMessage fileNamesRequest;
    fileNamesRequest.MID = static_cast<uint8_t>(MessageType::SEN_FILE_NAMES_REQ);
    GenNumReq request;
    request.id_token = 0;
    fileNamesRequest.content = request;

    std::cout << "Sending SEN_FILE_NAMES_REQ..." << std::endl;

    ssize_t sent = tempSocket->bSendData(tempSocket->getSocketFD(), fileNamesRequest);
    if (sent <= 0) {
        delete tempSocket;
        QMessageBox::critical(this, "Error", "No se pudo enviar la solicitud de nombres");
        return;
    }

    std::cout << "✓ Request sent, waiting for response..." << std::endl;

    // Recibir todos los nombres de archivos
    std::vector<std::string> allLogFiles;
    bool receivingPages = true;
    int pagesReceived = 0;

    while (receivingPages) {
        genMessage response;
        ssize_t received = tempSocket->bReceiveData(tempSocket->getSocketFD(), response);

        if (received <= 0) {
            std::cerr << "✗ Error receiving file names" << std::endl;
            break;
        }

        if (response.MID == static_cast<uint8_t>(MessageType::SEN_FILE_NAMES_RES)) {
            senFileNamesRes fileNamesRes = getMessageContent<senFileNamesRes>(response);
            pagesReceived++;

            std::cout << "✓ Received page " << (fileNamesRes.page + 1)
                      << " of " << fileNamesRes.totalPages << std::endl;

            for (const auto& fileReq : fileNamesRes.fileNames.names) {
                if (!fileReq.Filename.empty()) {
                    allLogFiles.push_back(fileReq.Filename);
                }
            }

            if (fileNamesRes.page + 1 >= fileNamesRes.totalPages) {
                receivingPages = false;
            }
        } else if (response.MID == static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG)) {
            errorCommonMsg errorMsg = getMessageContent<errorCommonMsg>(response);
            delete tempSocket;
            QMessageBox::critical(this, "Error", QString::fromStdString(errorMsg.message));
            return;
        }
    }

    // 🔴 IMPORTANTE: Cerrar la conexión después de recibir nombres
    delete tempSocket;
    tempSocket = nullptr;

    std::cout << "\nTotal files received: " << allLogFiles.size() << std::endl;

    if (allLogFiles.empty()) {
        QMessageBox::information(this, "Sin archivos", "No se encontraron archivos de logs");
        return;
    }

    // Filtrar archivos del sensor
    std::vector<std::string> sensorLogFiles;
    std::string cleanType = sensorType;
    cleanType.erase(std::remove(cleanType.begin(), cleanType.end(), ':'), cleanType.end());

    for (const auto& filename : allLogFiles) {
        if (filename.find(cleanType) == 0 && filename.find(".log") != std::string::npos) {
            sensorLogFiles.push_back(filename);
        }
    }

    if (sensorLogFiles.empty()) {
        QMessageBox::information(this, "Sin logs",
                                 QString("No se encontraron logs para el sensor tipo '%1'")
                                     .arg(QString::fromStdString(cleanType)));
        return;
    }

    std::cout << "\n✓ Found " << sensorLogFiles.size() << " log files for this sensor" << std::endl;

    // ========================================
    // PASO 2: Obtener contenido de cada archivo
    // ========================================
    QString allLogs;
    int totalEntries = 0;
    int filesProcessed = 0;

    for (const auto& logFile : sensorLogFiles) {
        std::cout << "\n[" << (filesProcessed + 1) << "/" << sensorLogFiles.size()
        << "] Requesting: " << logFile << std::endl;

        // 🟢 CREAR NUEVA CONEXIÓN PARA CADA ARCHIVO
        Socket* fileSocket = new Socket();

        if (!fileSocket->create()) {
            std::cerr << "✗ Could not create socket for file: " << logFile << std::endl;
            delete fileSocket;
            continue;
        }

        if (!fileSocket->connectToServer(IP, port)) {
            std::cerr << "✗ Could not connect for file: " << logFile << std::endl;
            delete fileSocket;
            continue;
        }

        // Solicitar bloques del archivo
        genMessage fileBlockReq;
        fileBlockReq.MID = static_cast<uint8_t>(MessageType::SEN_FILE_BLOCK_REQ);
        genSenFileReq blockRequest;
        blockRequest.id_token = 0;
        blockRequest.fileName.Filename = logFile;
        fileBlockReq.content = blockRequest;

        sent = fileSocket->bSendData(fileSocket->getSocketFD(), fileBlockReq);
        if (sent <= 0) {
            std::cerr << "✗ Could not request file: " << logFile << std::endl;
            delete fileSocket;
            continue;
        }

        // Recibir bloques
        bool receivingBlocks = true;
        QString fileContent;
        int blocksReceived = 0;

        while (receivingBlocks) {
            genMessage blockResponse;
            ssize_t blockReceived = fileSocket->bReceiveData(fileSocket->getSocketFD(), blockResponse);

            if (blockReceived <= 0) {
                std::cerr << "✗ Error receiving blocks" << std::endl;
                break;
            }

            if (blockResponse.MID == static_cast<uint8_t>(MessageType::SEN_FILE_BLOCK_RESP)) {
                senFileBlockRes blockRes = getMessageContent<senFileBlockRes>(blockResponse);
                blocksReceived++;

                std::cout << "  ✓ Block " << (blockRes.page + 1)
                          << "/" << blockRes.totalPages << std::endl;

                if (!blockRes.firstBlock.empty()) {
                    std::string block1 = blockRes.firstBlock;
                    size_t nullPos = block1.find('\0');
                    if (nullPos != std::string::npos) {
                        block1 = block1.substr(0, nullPos);
                    }
                    fileContent += QString::fromStdString(block1);
                }

                if (!blockRes.secondBlock.empty()) {
                    std::string block2 = blockRes.secondBlock;
                    size_t nullPos = block2.find('\0');
                    if (nullPos != std::string::npos) {
                        block2 = block2.substr(0, nullPos);
                    }
                    fileContent += QString::fromStdString(block2);
                }

                if (blockRes.page + 1 >= blockRes.totalPages) {
                    std::cout << "  ✓ Complete (" << blocksReceived << " blocks)" << std::endl;
                    receivingBlocks = false;
                }
            } else if (blockResponse.MID == static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG)) {
                errorCommonMsg errorMsg = getMessageContent<errorCommonMsg>(blockResponse);
                std::cerr << "  ✗ Error: " << errorMsg.message << std::endl;
                break;
            }
        }

        // 🔴 IMPORTANTE: Cerrar conexión después de cada archivo
        delete fileSocket;
        fileSocket = nullptr;

        if (!fileContent.isEmpty()) {
            allLogs += QString("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
            allLogs += QString("📁 Archivo: %1\n").arg(QString::fromStdString(logFile));
            allLogs += QString("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
            allLogs += fileContent;

            int entries = fileContent.count('\n');
            totalEntries += entries;
            filesProcessed++;
        }
    }

    std::cout << "\n✓ Total files processed: " << filesProcessed
              << "/" << sensorLogFiles.size() << std::endl;
    std::cout << "✓ Total entries: " << totalEntries << std::endl;

    // Mostrar resultados
    if (!allLogs.isEmpty()) {
        QString formattedLogs = QString(
                                    "═══════════════════════════════════════\n"
                                    "       HISTORIAL DEL SENSOR\n"
                                    "═══════════════════════════════════════\n\n"
                                    "Tipo:            %1\n"
                                    "IP:              %2\n"
                                    "Archivos:        %3\n"
                                    "Total registros: %4\n\n"
                                    "═══════════════════════════════════════\n"
                                    "DATOS HISTÓRICOS:\n"
                                    "═══════════════════════════════════════\n%5"
                                    ).arg(QString::fromStdString(cleanType))
                                    .arg(QString::fromStdString(sensorIP))
                                    .arg(filesProcessed)
                                    .arg(totalEntries)
                                    .arg(allLogs);

        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Historial - " + QString::fromStdString(cleanType));
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStandardButtons(QMessageBox::Ok);

        QTextEdit* textEdit = new QTextEdit();
        textEdit->setReadOnly(true);
        textEdit->setPlainText(formattedLogs);
        textEdit->setMinimumSize(700, 500);
        textEdit->setFont(QFont("Courier New", 9));

        msgBox.layout()->addWidget(textEdit);
        msgBox.exec();
    } else {
        QMessageBox::information(this, "Sin datos",
                                 QString("No se pudieron cargar datos de los %1 archivos encontrados")
                                     .arg(sensorLogFiles.size()));
    }

    std::cout << "==========================================\n" << std::endl;
}

void MenuWindow::askForServerStatus() {
    std::cout << "\n========== ASK FOR SERVER STATUS ==========" << std::endl;

    std::string IP = currentUser.getIP();
    int port = currentUser.getPort();

    std::cout << "Connecting to: " << IP << ":" << port << std::endl;

    Socket* tempSocket = nullptr;

    try {
        tempSocket = new Socket();

        if (!tempSocket->create()) {
            delete tempSocket;
            QMessageBox::critical(this, "Error", "No se pudo crear el socket");
            return;
        }

        if (!tempSocket->connectToServer(IP, port)) {
            delete tempSocket;
            QMessageBox::critical(this, "Error", "No se pudo conectar al servidor");
            return;
        }

        std::cout << "✓ Connected to master server" << std::endl;

        genMessage statusRequest;
        statusRequest.MID = static_cast<uint8_t>(MessageType::SERVER_STATUS_REQ);
        serverStatusReq request;
        request.id_token = 0;
        statusRequest.content = request;

        std::cout << "Sending SERVER_STATUS_REQ message..." << std::endl;

        ssize_t sent = tempSocket->bSendData(tempSocket->getSocketFD(), statusRequest);
        if (sent <= 0) {
            delete tempSocket;
            QMessageBox::critical(this, "Error", "No se pudo enviar la solicitud");
            return;
        }

        std::cout << "✓ Request sent (" << sent << " bytes)" << std::endl;
        std::cout << "Waiting for response..." << std::endl;

        genMessage response;
        ssize_t received = tempSocket->bReceiveData(tempSocket->getSocketFD(), response);

        if (received <= 0) {
            delete tempSocket;
            QMessageBox::critical(this, "Error", "No se pudo recibir respuesta");
            return;
        }

        std::cout << "✓ Response received (" << received << " bytes)" << std::endl;
        std::cout << "Response MID: " << static_cast<int>(response.MID) << std::endl;

        if (response.MID == static_cast<uint8_t>(MessageType::SERVER_STATUS_RES)) {
            try {
                serverStatusRes statusRes = getMessageContent<serverStatusRes>(response);

                std::cout << "\n========== RECEIVED SERVER STATUS ==========" << std::endl;
                std::cout << "Total servers: " << statusRes.servers.size() << std::endl;

                // Limpiar lista ANTES de agregar items
                this->ui->arduino_list->clear();

                // Agregar cada servidor a la lista
                for (const auto& server : statusRes.servers) {
                    QString statusIcon = server.isConnected ? "🟢" : "🔴";
                    QString statusText = server.isConnected ? "ONLINE" : "OFFLINE";

                    QString itemText = QString("%1 %2 - %3 (%4:%5)")
                                           .arg(statusIcon)
                                           .arg(QString::fromStdString(server.serverName))
                                           .arg(statusText)
                                           .arg(QString::fromStdString(server.serverIP))
                                           .arg(server.serverPort);

                    QListWidgetItem* item = new QListWidgetItem(itemText);

                    if (server.isConnected) {
                        item->setBackground(QColor(200, 255, 200));
                    } else {
                        item->setBackground(QColor(255, 200, 200));
                    }

                    this->ui->arduino_list->addItem(item);

                    std::cout << "  [" << server.serverName << "] "
                              << (server.isConnected ? "ONLINE" : "OFFLINE")
                              << " at " << server.serverIP << ":" << server.serverPort << std::endl;
                }

                delete tempSocket;
                std::cout << "✓ List updated with " << statusRes.servers.size() << " servers" << std::endl;
                std::cout << "==========================================\n" << std::endl;

            } catch (const std::runtime_error& e) {
                delete tempSocket;
                std::cerr << "✗ Error processing response: " << e.what() << std::endl;
                QMessageBox::critical(this, "Error", "Error al procesar respuesta del servidor");
            }
        } else if (response.MID == static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG)) {
            errorCommonMsg errorMsg = getMessageContent<errorCommonMsg>(response);
            delete tempSocket;
            std::cerr << "✗ Server error: " << errorMsg.message << std::endl;
            QMessageBox::critical(this, "Error", QString::fromStdString(errorMsg.message));
        } else {
            delete tempSocket;
            std::cerr << "✗ Unexpected response MID: " << static_cast<int>(response.MID) << std::endl;
            QMessageBox::critical(this, "Error", "Respuesta inesperada del servidor");
        }

    } catch (const std::exception& e) {
        if (tempSocket) delete tempSocket;
        std::cerr << "✗ Exception: " << e.what() << std::endl;
        QMessageBox::critical(this, "Error de conexión",
                              QString("No se pudo conectar al servidor: %1").arg(e.what()));
    }
}

void MenuWindow::on_pc1_button_clicked()
{
    DialogManagePC *dialog = new DialogManagePC(this);
    dialog->setPCIndex(0);  // PC 1
    dialog->exec();  // Abre el diálogo de forma modal
    delete dialog;
}

void MenuWindow::on_pc2_button_clicked()
{
    DialogManagePC *dialog = new DialogManagePC(this);
    dialog->setPCIndex(1);  // PC 2
    dialog->exec();
    delete dialog;
}

void MenuWindow::on_pc3_button_clicked()
{
    DialogManagePC *dialog = new DialogManagePC(this);
    dialog->setPCIndex(2);  // PC 3
    dialog->exec();
    delete dialog;
}

void MenuWindow::on_pc4_button_clicked()
{
    DialogManagePC *dialog = new DialogManagePC(this);
    dialog->setPCIndex(3);  // PC 4
    dialog->exec();
    delete dialog;
}

void MenuWindow::on_compus_list_itemClicked(QListWidgetItem *item)
{

}
