/// Copyright Los Más Fritos - 2025

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include "../Scenes/scene.hpp"

class Renderer {
  private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    int* p_SceneIndex;
    std::vector<Scene>* p_Scenes;

  public:
    Renderer(const char* title, int width, int height);
    ~Renderer();

    void draw();

    void setSceneIndex_ptr(int* scene_index) { p_SceneIndex = scene_index; }
    void setScenes_ptr(std::vector<Scene>* scenes) { p_Scenes = scenes; }
};

#endif // RENDERER_HPP