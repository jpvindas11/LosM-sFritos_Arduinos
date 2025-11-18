#include "dialogmanagepc.h"
#include "ui_dialogmanagepc.h"

DialogManagePC::DialogManagePC(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogManagePC)
{
    ui->setupUi(this);
}

DialogManagePC::~DialogManagePC()
{
    delete ui;
}

void DialogManagePC::on_on_button_clicked()
{
    QString selectedServer = ui->server_options->currentText();
    SSHClient ssh;

    if (!ssh.connect(targetPC, 22, labUser)) {
        QMessageBox::critical(this, "Error", "No se pudo conectar al PC");
        return;
    }

    if (!ssh.authenticate(labPass)) {
        QMessageBox::critical(this, "Error", "Error de autenticación");
        ssh.disconnect();
        return;
    }

    ssh_channel channel = ssh.createChannel();
    if (channel) {
        // Construir la ruta del ejecutable según el servidor
        std::string serverPath;
        std::string serverName = selectedServer.toLower().toStdString();

        if (serverName == "Autenticacion") {
            serverPath = "/home/lab3-5/servidores/autenticacion/authServer";
        } else if (serverName == "Almacenamiento") {
            serverPath = "/home/lab3-5/servidores/almacenamiento/storage";
        } else if (serverName == "Logs Usuarios") {
            serverPath = "/home/lab3-5/servidores/logs/UserLogs";
        } else if (serverName == "Proxy") {
            serverPath = "/home/lab3-5/servidores/proxy/proxy";
        }

        // Ejecutar en background
        std::string command = serverPath + " &";

        if (ssh.executeCommand(channel, command, true)) {
            QMessageBox::information(this, "Éxito",
                                     QString("Servidor %1 iniciado correctamente").arg(selectedServer));
        } else {
            QMessageBox::critical(this, "Error", "No se pudo ejecutar el comando");
        }
    }

    ssh.disconnect();
}

void DialogManagePC::on_off_button_clicked()
{
    QString selectedServer = ui->server_options->currentText();
    SSHClient ssh;

    if (!ssh.connect(targetPC, 22, labUser)) {
        QMessageBox::critical(this, "Error", "No se pudo conectar al PC");
        return;
    }

    if (!ssh.authenticate(labPass)) {
        QMessageBox::critical(this, "Error", "Error de autenticación");
        ssh.disconnect();
        return;
    }

    ssh_channel channel = ssh.createChannel();
    if (channel) {
        std::string serverName = selectedServer.toLower().toStdString();

        // Matar el proceso por nombre
        std::string command = "pkill -f " + serverName + "_server";

        if (ssh.executeCommand(channel, command)) {
            QMessageBox::information(this, "Éxito",
                                     QString("Servidor %1 detenido correctamente").arg(selectedServer));
        } else {
            QMessageBox::critical(this, "Error", "No se pudo ejecutar el comando");
        }
    }

    ssh.disconnect();
}

void DialogManagePC::on_exit_button_clicked()
{
    this->close();
}

void DialogManagePC::setPCIndex(int index) {
    switch(index) {
    case 0:
        this->ui->title->setText("PC 1");
        this->targetPC = "10.1.137.7";
        break;
    case 1:
        this->ui->title->setText("PC 2");
        this->targetPC = "10.1.137.7";
        break;
    case 2:
        this->ui->title->setText("PC 3");
        this->targetPC = "10.1.137.7";
        break;
    case 3:
        this->ui->title->setText("PC 4");
        this->targetPC = "10.1.137.7";
        break;
    default:
        this->close();
        break;
    }

    this->index = index;
}
