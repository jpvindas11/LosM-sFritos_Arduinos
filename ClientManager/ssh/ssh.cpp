#include "ssh.hpp"

bool SSHClient::connect(std::string host, int port, std::string user) {
    // Implementación de la conexión SSH
    this->session = ssh_new();
    if (this->session == nullptr) {
        return false;
    }

    ssh_options_set(this->session, SSH_OPTIONS_HOST, host.c_str());
    ssh_options_set(this->session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(this->session, SSH_OPTIONS_USER, user.c_str());

    int connection_result = ssh_connect(this->session);
    if (connection_result != SSH_OK) {
        ssh_free(this->session);
        return false;
    }

    return true;
}

bool SSHClient::authenticate(std::string password) {
    // Implementación de la autenticación SSH
    if (this->session == nullptr) {
        return false;
    }

    int auth_result = ssh_userauth_password(this->session, nullptr, password.c_str());
    if (auth_result != SSH_AUTH_SUCCESS) {
        ssh_disconnect(this->session);
        ssh_free(this->session);
        return false;
    }

    return true;
}

ssh_channel SSHClient::createChannel() {
    // Implementación de la creación de canal SSH
    if (this->session == nullptr) {
        return nullptr;
    }

    this->channel = ssh_channel_new(this->session);
    if (this->channel == nullptr) {
        return nullptr;
    }

    return this->channel;
}

bool SSHClient::executeCommand(ssh_channel channel, const std::string& command, bool background) {
    // Implementación de la ejecución de comando SSH
    if (channel == nullptr) {
        return false;
    }

    int open_result = ssh_channel_open_session(channel);
    if (open_result != SSH_OK) {
        return false;
    }

    // Si es background, usar bash para ejecutar en background con disown
    std::string exec_command = command;
    if (background) {
        exec_command = "bash -c '(" + command + ") > /dev/null 2>&1 & disown'";
    }

    int exec_result = ssh_channel_request_exec(channel, exec_command.c_str());
    if (exec_result != SSH_OK) {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return false;
    }

    // Siempre leer hasta que el canal termine para evitar que se quede colgado
    char buffer[256];
    int bytes_read;
    while ((bytes_read = ssh_channel_read(channel, buffer, sizeof(buffer), 0)) > 0) {
        if (!background) {
            fwrite(buffer, 1, bytes_read, stdout);
        }
    }

    while ((bytes_read = ssh_channel_read(channel, buffer, sizeof(buffer), 1)) > 0) {
        if (!background) {
            fwrite(buffer, 1, bytes_read, stderr);
        }
    }

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    
    // Marcar el canal como cerrado
    if (this->channel == channel) {
        this->channel = nullptr;
    }

    return true;
}

void SSHClient::disconnect() {
    // Implementación de la desconexión SSH
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
