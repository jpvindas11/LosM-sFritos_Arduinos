#include <csignal>

#include "Handler.hpp"
#include "SensorServer.hpp"

int main() {
  signal(SIGINT, Handler::signalHandler);
  signal(SIGTERM, Handler::signalHandler);

  SensorServer& server = SensorServer::getInstance();

  server.run("127.0.0.1", 9090, "127.0.0.1", 9090);

  return EXIT_SUCCESS;
}