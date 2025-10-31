#include <csignal>

#include "Handler.hpp"
#include "LogsServer.hpp"

int main() {
  signal(SIGINT, Handler::signalHandler);
  signal(SIGTERM, Handler::signalHandler);

  LogsServer& server = LogsServer::getInstance();

  server.run("127.0.0.1", 9090, "127.0.0.1", 9090);

  return EXIT_SUCCESS;
}