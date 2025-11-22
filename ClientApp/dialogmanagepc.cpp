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
        // Construir la ruta COMPLETA del ejecutable según el servidor
        std::string serverPath;
        std::string serverName = selectedServer.toStdString();

        if (serverName == "Autenticacion") {
            serverPath = "cd ~/servers/autenticacion/bin && ./authServer";
        } else if (serverName == "Almacenamiento") {
            serverPath = "cd ~/servers/storage/bin && ./storage";
        } else if (serverName == "Logs Usuarios") {
            serverPath = "cd ~/servers/UserLogs/bin && ./UserLogs";
        } else if (serverName == "Proxy") {
            serverPath = "cd ~/servers/Proxy/bin && ./proxy";
        }
        std::string command = serverPath;

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
        std::string serverName = selectedServer.toStdString();
        std::string processName;

        if (serverName == "Autenticacion") {
            processName = "authServer";
        } else if (serverName == "Almacenamiento") {
            processName = "storage";
        } else if (serverName == "Logs Usuarios") {
            processName = "UserLogs";
        } else if (serverName == "Proxy") {
            processName = "proxy";
        }

        // SOLUCIÓN 1: Usar pkill con -9 (SIGKILL) y verificar con echo
        std::string command = "pkill -9 -f " + processName + " && echo 'Killed' || echo 'Not found'";

        // SOLUCIÓN 2: Alternativa más robusta
        // std::string command = "killall -9 " + processName + " 2>&1 || echo 'Process not running'";

        if (ssh.executeCommand(channel, command, false)) {
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
        this->targetPC = "10.1.35.11";
        break;
    case 1:
        this->ui->title->setText("PC 2");
        this->targetPC = "10.1.35.12";
        break;
    case 2:
        this->ui->title->setText("PC 3");
        this->targetPC = "10.1.35.9";
        break;
    case 3:
        this->ui->title->setText("PC 4");
        this->targetPC = "10.1.35.10";
        break;
    default:
        this->close();
        break;
    }

    this->index = index;
}
