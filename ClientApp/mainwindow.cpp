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

void MainWindow::on_pushButton_ip_clicked()
{
    QString ip = ui->lineEdit_IP->text();
    QString port = ui->lineEdit_PORT->text();

    // Validar que los campos no estén vacíos
    if (ip.isEmpty() || port.isEmpty()) {
        QMessageBox::warning(this, "Error", "Por favor ingrese IP y Puerto");
        return;
    }

    // Crear el socket
    if (!socket->create()) {
        QMessageBox::critical(this, "Error", "No se pudo crear el socket");
        return;
    }

    // Conectar al servidor
    if (!socket->connectToServer(ip.toStdString(), port.toInt())) {
        QMessageBox::critical(this, "Error", "No se pudo conectar al servidor");
        ui->lineEdit_IP->clear();
        ui->lineEdit_PORT->clear();
        isConnected = false;
        return;
    }

    isConnected = true;
    this->currentData.setNetwork(ip.toStdString(), port.toInt());

    QMessageBox::information(this, "Éxito", "Conectado al servidor");
}

void MainWindow::on_pushButton_clicked()
{
    if (!isConnected) {
        QMessageBox::warning(this, "Error", "Primero debe conectarse al servidor");
        return;
    }

    QString user = ui->lineEdit_user->text();
    QString pass = ui->lineEdit_pass->text();

    if (user.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Error", "Por favor ingrese usuario y contraseña");
        return;
    }

    // Crear mensaje de login usando Bitsery
    genMessage loginRequest;
    loginRequest.MID = static_cast<uint8_t>(MessageType::AUTH_LOGIN_REQ);

    authLoginReq authReq;
    authReq.user = user.toStdString();
    authReq.pass = pass.toStdString();
    loginRequest.content = authReq;

    // Enviar solicitud de login
    ssize_t sent = socket->bSendData(socket->getSocketFD(), loginRequest);
    if (sent <= 0) {
        QMessageBox::critical(this, "Error", "No se pudo enviar la solicitud de login");
        ui->lineEdit_user->clear();
        ui->lineEdit_pass->clear();
        return;
    }

    // Recibir respuesta
    genMessage response;
    ssize_t received = socket->bReceiveData(socket->getSocketFD(), response);

    if (received <= 0) {
        QMessageBox::critical(this, "Error", "No se pudo recibir respuesta del servidor");
        ui->lineEdit_user->clear();
        ui->lineEdit_pass->clear();
        return;
    }

    // Verificar tipo de respuesta
    if (response.MID == static_cast<uint8_t>(MessageType::AUTH_LOGIN_SUCCESS)) {
        try {
            authLoginSuccess loginSuccess = getMessageContent<authLoginSuccess>(response);

            ui->login_info->setText("Ingreso exitoso");

            // Configurar datos del usuario
            userDataQt login;
            login.setData(user.toStdString(), pass.toStdString(), loginSuccess.Token.userType);

            // Crear y mostrar ventana de menú
            MenuWindow* menu = new MenuWindow();
            menu->setCurrentUser(login);
            // menu->setSocket(socket.get()); // Pasar el socket al menú
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
            ui->login_info->setText("El usuario o contraseña son incorrectos");
        }
    }

    ui->lineEdit_user->clear();
    ui->lineEdit_pass->clear();
}

