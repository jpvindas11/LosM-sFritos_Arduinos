#include <csignal>

#include "Handler.hpp"
#include "LogsServer.hpp"
#include "../Util/IPConstants.hpp"

int main() {
  signal(SIGINT, Handler::signalHandler);
  signal(SIGTERM, Handler::signalHandler);

  LogsServer& server = LogsServer::getInstance();

  server.run("0.0.0.0", PORT_MASTER_LOGS);

  return EXIT_SUCCESS;
}