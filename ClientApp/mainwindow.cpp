#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menuwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), isConnected(false)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_ip_clicked()
{
    ServerDiscover discoverer(DISC_MASTER);
    std::string ipGrab = discoverer.lookForServer();

    // Crear socket temporal para verificar conexión
    Socket testSocket;

    if (!testSocket.create()) {
        QMessageBox::critical(this, "Error", "No se pudo crear el socket");
        return;
    }

    // Intentar conectar
    if (!testSocket.connectToServer(ipGrab, PORT_MASTER_USERS)) {
        QMessageBox::critical(this, "Error", "No se pudo conectar al servidor");
        isConnected = false;
        testSocket.closeSocket();
        return;
    }

    // Conexión exitosa - guardar datos y cerrar
    isConnected = true;
    testSocket.closeSocket();

    this->currentData.setNetwork(ipGrab, PORT_MASTER_USERS);
    QMessageBox::information(this, "Éxito", "Servidor disponible");
}

void MainWindow::on_pushButton_clicked()
{
    if (!isConnected) {
        QMessageBox::warning(this, "Error", "Primero debe verificar la conexión al servidor");
        return;
    }

    QString user = ui->lineEdit_user->text();
    QString pass = ui->lineEdit_pass->text();

    if (user.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Error", "Por favor ingrese usuario y contraseña");
        return;
    }

    // Crear nuevo socket para login
    Socket loginSocket;

    if (!loginSocket.create()) {
        QMessageBox::critical(this, "Error", "No se pudo crear el socket");
        return;
    }

    // Conectar usando los datos guardados
    if (!loginSocket.connectToServer(currentData.getIP(), currentData.getPort())) {
        QMessageBox::critical(this, "Error", "No se pudo conectar al servidor");
        ui->lineEdit_user->clear();
        ui->lineEdit_pass->clear();
        isConnected = false;
        loginSocket.closeSocket();
        return;
    }

    // Crear mensaje de login
    genMessage loginRequest;
    loginRequest.MID = static_cast<uint8_t>(MessageType::AUTH_LOGIN_REQ);

    authLoginReq authReq;
    authReq.user = user.toStdString();
    authReq.pass = pass.toStdString();
    loginRequest.content = authReq;

    // Enviar solicitud
    ssize_t sent = loginSocket.bSendData(loginSocket.getSocketFD(), loginRequest);
    if (sent <= 0) {
        QMessageBox::critical(this, "Error", "No se pudo enviar la solicitud de login");
        ui->lineEdit_user->clear();
        ui->lineEdit_pass->clear();
        loginSocket.closeSocket();
        return;
    }

    // Recibir respuesta
    genMessage response;
    ssize_t received = loginSocket.bReceiveData(loginSocket.getSocketFD(), response);

    // Cerrar socket después de recibir respuesta
    loginSocket.closeSocket();

    if (received <= 0) {
        QMessageBox::critical(this, "Error", "No se pudo recibir respuesta del servidor");
        ui->lineEdit_user->clear();
        ui->lineEdit_pass->clear();
        return;
    }

    // Procesar respuesta
    if (response.MID == static_cast<uint8_t>(MessageType::AUTH_LOGIN_SUCCESS)) {
        try {
            authLoginSuccess loginSuccess = getMessageContent<authLoginSuccess>(response);
            ui->login_info->setText("Ingreso exitoso");

            currentData.setData(user.toStdString(), pass.toStdString(), loginSuccess.Token.userType);

            MenuWindow* menu = new MenuWindow();
            menu->setCurrentUser(currentData);
            menu->show();
            this->hide();

        } catch (const std::runtime_error& e) {
            QMessageBox::critical(this, "Error", "Error al procesar respuesta del servidor");
        }
    }
    else if (response.MID == static_cast<uint8_t>(MessageType::ERR_COMMOM_MSG)) {
        try {
            errorCommonMsg errorMsg = getMessageContent<errorCommonMsg>(response);
            ui->login_info->setText("Error: " + QString::fromStdString(errorMsg.message));
        } catch (const std::runtime_error& e) {
            ui->login_info->setText("Usuario o contraseña incorrectos");
        }
    }

    ui->lineEdit_user->clear();
    ui->lineEdit_pass->clear();
}
