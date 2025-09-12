/// Copyright Los Más Fritos - 2025

#ifndef CLIENTMANAGER_HPP
#define CLIENTMANAGER_HPP

#include <vector>
#include "../Scenes/scene.hpp"
#include "../Definitions/colors_def.hpp"
#include "../Rendering/renderer.hpp"

class ClientManager {
  private:

    /// @brief Stores a list of scenes, containing elements for a menu
    std::vector<Scene> scenes;
    int current_scene_index;

    Renderer renderer;

  public:
    ClientManager();
    ~ClientManager();

    void run();
};

#endif