#ifndef HANDLER_HPP
#define HANDLER_HPP

#include "LogsServer.hpp"

class Handler{
 public:
    /// @brief free static method to stop the server execution
    static void signalHandler(int signal);
};

#endif  // HANDLER