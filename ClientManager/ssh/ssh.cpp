#include "ssh.hpp"

bool SSHClient::connect(std::string host, int port, std::string user) {
    this->session = ssh_new();
    if (this->session == nullptr) {
        this->lastError = "Failed to create SSH session";
        return false;
    }

    ssh_options_set(this->session, SSH_OPTIONS_HOST, host.c_str());
    ssh_options_set(this->session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(this->session, SSH_OPTIONS_USER, user.c_str());

    // CRÍTICO: Aceptar host keys automáticamente
    int stricthostkey = 0;
    ssh_options_set(this->session, SSH_OPTIONS_STRICTHOSTKEYCHECK, &stricthostkey);

    // Timeout de conexión
    long timeout = 10; // 10 segundos
    ssh_options_set(this->session, SSH_OPTIONS_TIMEOUT, &timeout);

    int connection_result = ssh_connect(this->session);
    if (connection_result != SSH_OK) {
        this->lastError = std::string("Connection failed: ") + ssh_get_error(this->session);
        ssh_free(this->session);
        this->session = nullptr;
        return false;
    }

    this->lastError = "";
    return true;
}

bool SSHClient::authenticate(std::string password) {
    if (this->session == nullptr) {
        this->lastError = "No active session";
        return false;
    }

    int auth_result = ssh_userauth_password(this->session, nullptr, password.c_str());
    if (auth_result != SSH_AUTH_SUCCESS) {
        this->lastError = std::string("Authentication failed: ") + ssh_get_error(this->session);
        ssh_disconnect(this->session);
        ssh_free(this->session);
        this->session = nullptr;
        return false;
    }

    this->lastError = "";
    return true;
}

ssh_channel SSHClient::createChannel() {
    if (this->session == nullptr) {
        this->lastError = "No active session";
        return nullptr;
    }

    this->channel = ssh_channel_new(this->session);
    if (this->channel == nullptr) {
        this->lastError = "Failed to create channel";
        return nullptr;
    }

    return this->channel;
}

bool SSHClient::executeCommand(ssh_channel channel, const std::string& command, bool background) {
    if (channel == nullptr) {
        this->lastError = "Invalid channel";
        return false;
    }

    int open_result = ssh_channel_open_session(channel);
    if (open_result != SSH_OK) {
        this->lastError = "Failed to open channel session";
        return false;
    }

    std::string exec_command = command;
    if (background) {
        exec_command = "bash -c '(" + command + ") > /dev/null 2>&1 & disown'";
    }

    int exec_result = ssh_channel_request_exec(channel, exec_command.c_str());
    if (exec_result != SSH_OK) {
        this->lastError = "Failed to execute command";
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return false;
    }

    // Capturar salida stdout
    this->commandOutput = "";
    this->commandError = "";

    char buffer[256];
    int bytes_read;

    // Leer stdout
    while ((bytes_read = ssh_channel_read(channel, buffer, sizeof(buffer), 0)) > 0) {
        this->commandOutput.append(buffer, bytes_read);
    }

    // Leer stderr
    while ((bytes_read = ssh_channel_read(channel, buffer, sizeof(buffer), 1)) > 0) {
        this->commandError.append(buffer, bytes_read);
    }

    // Obtener código de salida
    ssh_channel_send_eof(channel);
    int exit_status = ssh_channel_get_exit_status(channel);

    ssh_channel_close(channel);
    ssh_channel_free(channel);

    if (this->channel == channel) {
        this->channel = nullptr;
    }

    this->exitStatus = exit_status;

    // Si hay error, guardarlo
    if (exit_status != 0 || !this->commandError.empty()) {
        this->lastError = "Command failed with exit code " + std::to_string(exit_status);
        if (!this->commandError.empty()) {
            this->lastError += ": " + this->commandError;
        }
    } else {
        this->lastError = "";
    }

    return exit_status == 0;
}

void SSHClient::disconnect() {
    if (this->channel != nullptr) {
        ssh_channel_free(this->channel);
        this->channel = nullptr;
    }

    if (this->session != nullptr) {
        ssh_disconnect(this->session);
        ssh_free(this->session);
        this->session = nullptr;
    }
}

std::string SSHClient::getLastError() const {
    return this->lastError;
}

std::string SSHClient::getCommandOutput() const {
    return this->commandOutput;
}

std::string SSHClient::getCommandError() const {
    return this->commandError;
}

int SSHClient::getExitStatus() const {
    return this->exitStatus;
}
