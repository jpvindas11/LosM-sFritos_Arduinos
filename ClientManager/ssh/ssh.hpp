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

private:
    ssh_session session;
    ssh_channel channel;
};

#endif