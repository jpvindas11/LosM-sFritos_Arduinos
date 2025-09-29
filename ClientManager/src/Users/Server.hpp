/// @copyright Los Más Fritos - 2025

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>

#include "common.hpp"
#include "Thread.hpp"
#include "Semaphore.hpp"

class Server : public virtual Thread {
  DISABLE_COPY(Server);

 protected:
  std::string message;
  Semaphore canCheckMessage;

 public:
  explicit Server()
    : canCheckMessage(0) {
  }

  virtual ~Server() {}

  virtual void waitForMessage() {
    while (true) {
      this->canCheckMessage.wait();
      if (message == "closeServer") {
        break;
      }
      this->processMessage();
    }
  }

  void setMessage(std::string& message) {
    this->message = message;
  }

  void checkMessage() {
    this->canCheckMessage.signal();
  }

  int run() override {
    this->waitForMessage();
    return EXIT_SUCCESS;
  }

  virtual void processMessage() = 0;
  virtual void sendMessage() = 0;

};

#endif  // SERVER_HPP
