// Copyright [2025] <Isaac Shih, Luis Eduardo Valverde, Stephannie Montanaro>
#include <iostream>

#include "Handler.hpp"


void Handler::signalHandler(int signal) {
  // report the recived signal and calls stop from server
  if (signal == 2) {
    std::cerr <<"  Ctrl+C  " <<std::endl;
  } else if (signal == 15) {
    std::cerr <<"  kill PID  " <<std::endl;
  }
  return SensorServer::getInstance().closeListeningSocket();
}

