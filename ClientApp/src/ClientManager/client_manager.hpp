/// Copyright Los Más Fritos - 2025

#ifndef CLIENTMANAGER_HPP
#define CLIENTMANAGER_HPP

#include <vector>
#include "../Scenes/scene.hpp"

class ClientManager {
  private:
    std::vector<Scene> scenes;
  public:
    ClientManager();
    ~ClientManager();

    void run();
};

#endif