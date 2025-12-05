#include "dialogmanagepc.h"
#include "ui_dialogmanagepc.h"
#include <chrono>
#include <thread>
#include <QFile>
#include <QTextStream>

DialogManagePC::DialogManagePC(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogManagePC)
{
    ui->setupUi(this);
    loadIPsFromFile();
}

DialogManagePC::~DialogManagePC()
{
    delete ui;
}

void DialogManagePC::loadIPsFromFile() {
    QString exePath = QCoreApplication::applicationDirPath();
    QString configFile = exePath + "/ips.txt";

    QFile file(configFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        pcIPs = {"10.1.35.11", "10.1.35.12", "10.1.35.9", "10.1.35.10"};

        // Crear el archivo con las IPs por defecto
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "10.1.35.11\n";
            out << "10.1.35.12\n";
            out << "10.1.35.9\n";
            out << "10.1.35.10\n";
            file.close();
        }
        return;
    }

    QTextStream in(&file);
    pcIPs.clear();

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            pcIPs.push_back(line.toStdString());
        }
    }

    file.close();

    while (pcIPs.size() < 4) {
        pcIPs.push_back("127.0.0.1");
    }
}

void DialogManagePC::showStyledMessage(const QString& title, const QString& message, bool isError) {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.setIcon(isError ? QMessageBox::Critical : QMessageBox::Information);

    // Aplicar estilo para que sea visible
    msgBox.setStyleSheet(
        "QMessageBox {"
        "   background-color: #1a233e;"
        "   color: #ffffff;"
        "}"
        "QMessageBox QLabel {"
        "   color: #ffffff;"
        "   font-size: 13px;"
        "   min-width: 300px;"
        "}"
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                               stop:0 #7b2ff7, stop:1 #00d4ff);"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 8px 20px;"
        "   color: white;"
        "   font-size: 12px;"
        "   font-weight: bold;"
        "   min-width: 80px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                               stop:0 #9351ff, stop:1 #00e5ff);"
        "}"
        );

    msgBox.exec();
}

void DialogManagePC::on_on_button_clicked()
{
    QString selectedServer = ui->server_options->currentText();
    SSHClient ssh;

    // Conectar
    if (!ssh.connect(targetPC, 22, labUser)) {
        showStyledMessage("Error de Conexión",
                          QString("No se pudo conectar a %1\n\nDetalles: %2")
                              .arg(QString::fromStdString(targetPC))
                              .arg(QString::fromStdString(ssh.getLastError())),
                          true);
        return;
    }

    // Autenticar
    if (!ssh.authenticate(labPass)) {
        showStyledMessage("Error de Autenticación",
                          QString("Fallo la autenticación\n\nDetalles: %1")
                              .arg(QString::fromStdString(ssh.getLastError())),
                          true);
        ssh.disconnect();
        return;
    }

    ssh_channel homeChannel = ssh.createChannel();
    std::string getHomeCmd = "echo $HOME";
    std::string homeDir;

    if (homeChannel && ssh.executeCommand(homeChannel, getHomeCmd, false)) {
        homeDir = ssh.getCommandOutput();
        homeDir.erase(homeDir.find_last_not_of(" \n\r\t") + 1);
    } else {
        showStyledMessage("Error", "No se pudo obtener el directorio home", true);
        ssh.disconnect();
        return;
    }

    std::string serverPath;
    std::string processName;
    std::string serverDir;
    std::string serverName = selectedServer.toStdString();

    if (serverName == "Autenticacion") {
        processName = "authServer";
        serverDir = homeDir + "/servers/authenticacion/bin";
        serverPath = serverDir + "/authServer";
    } else if (serverName == "Almacenamiento") {
        processName = "storage";
        serverDir = homeDir + "/servers/storage/bin";
        serverPath = serverDir + "/storage";
    } else if (serverName == "Logs Usuarios") {
        processName = "UserLogs";
        serverDir = homeDir + "/servers/UserLogs/bin";
        serverPath = serverDir + "/UserLogs";
    } else if (serverName == "Proxy") {
        processName = "proxy";
        serverDir = homeDir + "/servers/Proxy/bin";
        serverPath = serverDir + "/proxy";
    }

    if (processName.empty() || serverDir.empty()) {
        showStyledMessage("Error de Configuración",
                          QString("Servidor '%1' no reconocido").arg(selectedServer),
                          true);
        ssh.disconnect();
        return;
    }

    std::string checkCmd = "pgrep -x " + processName + " > /dev/null && echo 'running' || echo 'stopped'";
    ssh_channel checkChannel = ssh.createChannel();

    if (checkChannel && ssh.executeCommand(checkChannel, checkCmd, false)) {
        std::string status = ssh.getCommandOutput();
        status.erase(status.find_last_not_of(" \n\r\t") + 1);

        if (status == "running") {
            showStyledMessage("Servidor Ya Activo",
                              QString("El servidor %1 ya está corriendo").arg(selectedServer),
                              false);
            ssh.disconnect();
            return;
        }
    }

    QString debugInfo;

    std::string verifyCmd = "ls -la " + serverPath + " 2>&1";
    ssh_channel verifyChannel = ssh.createChannel();

    if (verifyChannel && ssh.executeCommand(verifyChannel, verifyCmd, false)) {
        std::string verifyOutput = ssh.getCommandOutput();
        debugInfo += "=== VERIFICACIÓN DEL EJECUTABLE ===\n";
        debugInfo += QString::fromStdString(verifyOutput) + "\n\n";
    }

    std::string testRunCmd = "timeout 2 " + serverPath + " 2>&1 || echo \"Exit code: $?\"";

    ssh_channel testChannel = ssh.createChannel();

    if (testChannel && ssh.executeCommand(testChannel, testRunCmd, false)) {
        std::string testOutput = ssh.getCommandOutput();
        debugInfo += "=== PRUEBA DE EJECUCIÓN DIRECTA ===\n";
        debugInfo += QString::fromStdString(testOutput) + "\n";

        // Si hay error obvio, mostrarlo
        if (testOutput.find("Permission denied") != std::string::npos ||
            testOutput.find("cannot execute") != std::string::npos ||
            testOutput.find("No such file") != std::string::npos) {

            showStyledMessage("Error al Ejecutar Servidor",
                              QString("El servidor %1 falló al ejecutarse:\n\n%2")
                                  .arg(selectedServer)
                                  .arg(debugInfo),
                              true);
            ssh.disconnect();
            return;
        }
    }

    std::string startCmd = "nohup " + serverPath + " > /dev/null 2>&1 & echo $!";

    ssh_channel startChannel = ssh.createChannel();

    if (startChannel && ssh.executeCommand(startChannel, startCmd, false)) {
        std::string output = ssh.getCommandOutput();
        std::string pidStr = output;
        pidStr.erase(pidStr.find_last_not_of(" \n\r\t") + 1);

        debugInfo += "\n=== INICIO DEL PROCESO ===\n";
        debugInfo += "PID: " + QString::fromStdString(pidStr) + "\n";

        // Esperar un momento
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // Verificar que realmente inició
        ssh_channel verifyChannel2 = ssh.createChannel();
        if (verifyChannel2 && ssh.executeCommand(verifyChannel2, checkCmd, false)) {
            std::string status = ssh.getCommandOutput();
            status.erase(status.find_last_not_of(" \n\r\t") + 1);

            if (status == "running") {
                showStyledMessage("Éxito",
                                  QString("Servidor %1 iniciado correctamente en %2")
                                      .arg(selectedServer)
                                      .arg(QString::fromStdString(targetPC)),
                                  false);
            } else {
                showStyledMessage("Error - El Servidor No Pudo Iniciar",
                                  QString("El servidor %1 no logró iniciar.\n\n%2")
                                      .arg(selectedServer)
                                      .arg(debugInfo),
                                  true);
            }
        }
    } else {
        showStyledMessage("Error al Iniciar",
                          QString("No se pudo ejecutar el comando de inicio\n\nDetalles: %1")
                              .arg(QString::fromStdString(ssh.getLastError())),
                          true);
    }

    ssh.disconnect();
}

void DialogManagePC::on_off_button_clicked()
{
    QString selectedServer = ui->server_options->currentText();
    SSHClient ssh;

    // Conectar
    if (!ssh.connect(targetPC, 22, labUser)) {
        showStyledMessage("Error de Conexión",
                          QString("No se pudo conectar a %1\n\nDetalles: %2")
                              .arg(QString::fromStdString(targetPC))
                              .arg(QString::fromStdString(ssh.getLastError())),
                          true);
        return;
    }

    // Autenticar
    if (!ssh.authenticate(labPass)) {
        showStyledMessage("Error de Autenticación",
                          QString("Fallo la autenticación\n\nDetalles: %1")
                              .arg(QString::fromStdString(ssh.getLastError())),
                          true);
        ssh.disconnect();
        return;
    }

    ssh_channel channel = ssh.createChannel();
    if (!channel) {
        showStyledMessage("Error",
                          QString("No se pudo crear el canal SSH\n\nDetalles: %1")
                              .arg(QString::fromStdString(ssh.getLastError())),
                          true);
        ssh.disconnect();
        return;
    }

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

    // Primero verificar si está corriendo
    std::string checkCmd = "pgrep -x " + processName + " > /dev/null && echo 'running' || echo 'stopped'";

    if (ssh.executeCommand(channel, checkCmd, false)) {
        std::string status = ssh.getCommandOutput();
        status.erase(status.find_last_not_of(" \n\r\t") + 1);

        if (status == "stopped") {
            showStyledMessage("Servidor No Activo",
                              QString("El servidor %1 no está corriendo").arg(selectedServer),
                              false);
            ssh.disconnect();
            return;
        }
    }

    // Detener el proceso
    std::string killCmd = "pkill -9 " + processName + " && echo 'killed' || echo 'failed'";
    ssh_channel killChannel = ssh.createChannel();

    if (killChannel && ssh.executeCommand(killChannel, killCmd, false)) {
        std::string result = ssh.getCommandOutput();
        result.erase(result.find_last_not_of(" \n\r\t") + 1);

        if (result == "killed") {
            // Verificar que realmente se detuvo
            std::this_thread::sleep_for(std::chrono::milliseconds(200));

            ssh_channel verifyChannel = ssh.createChannel();
            if (verifyChannel && ssh.executeCommand(verifyChannel, checkCmd, false)) {
                std::string status = ssh.getCommandOutput();
                status.erase(status.find_last_not_of(" \n\r\t") + 1);

                if (status == "stopped") {
                    showStyledMessage("Éxito",
                                      QString("Servidor %1 detenido correctamente").arg(selectedServer),
                                      false);
                } else {
                    showStyledMessage("Advertencia",
                                      QString("Se envió la señal de detención pero el proceso aún aparece activo"),
                                      true);
                }
            }
        } else {
            showStyledMessage("Error",
                              QString("No se pudo detener el servidor %1").arg(selectedServer),
                              true);
        }
    } else {
        showStyledMessage("Error al Detener",
                          QString("No se pudo ejecutar el comando\n\nDetalles: %1")
                              .arg(QString::fromStdString(ssh.getLastError())),
                          true);
    }

    ssh.disconnect();
}

void DialogManagePC::on_exit_button_clicked()
{
    this->close();
}

void DialogManagePC::setPCIndex(int index) {
    if (index < 0 || index >= static_cast<int>(pcIPs.size())) {
        this->close();
        return;
    }

    this->targetPC = pcIPs[index];
    this->ui->title->setText(QString("PC %1").arg(index + 1));
    this->index = index;
}
