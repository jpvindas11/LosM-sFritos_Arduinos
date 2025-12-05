#ifndef SSH_HPP
#define SSH_HPP

#include <libssh/libssh.h>
#include <string>
#include <cstdio>

class SSHClient {
public:
    bool connect(std::string host, int port, std::string user);
    bool authenticate(std::string password);
    ssh_channel createChannel();
    bool executeCommand(ssh_channel channel, const std::string& command, bool background = false);
    void disconnect();

    // Nuevas funciones para obtener información de errores
    std::string getLastError() const;
    std::string getCommandOutput() const;
    std::string getCommandError() const;
    int getExitStatus() const;

private:
    ssh_session session = nullptr;
    ssh_channel channel = nullptr;
    std::string lastError;
    std::string commandOutput;
    std::string commandError;
    int exitStatus = 0;
};

#endif
