#include <csignal>

#include "../Util/IPConstants.hpp"
#include "Handler.hpp"
#include "SensorServer.hpp"

int main() {
  signal(SIGINT, Handler::signalHandler);
  signal(SIGTERM, Handler::signalHandler);

  SensorServer& server = SensorServer::getInstance();

  server.run("0.0.0.0", PORT_MASTER_STORAGE);

  return EXIT_SUCCESS;
}