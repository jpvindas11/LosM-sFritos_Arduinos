/// Copyright Los Más Fritos - 2025

#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include "../Definitions/colors_def.hpp"
#include "button.hpp"
#include "element.hpp"

typedef enum SCENE_NAME {
    SCENE_STARTUP,
    SCENE_LOGIN,
    SCENE_OPTIONS,
    SCENE_STORAGE,
    SCENE_USERS,
    SCENE_SOFTWARE,
    SCENE_ARDUINO,
    SCENE_AMOUNT,
} SCENE_NAME;

class Scene {
  private:
    std::vector<Element> elements;
    std::vector<Button> buttons;

    COLOR background_color;
    SCENE_NAME name;
  public:
    Scene(SCENE_NAME name, COLOR bg_color);
    ~Scene();

    void add_element(Element element);
    void add_button(Button button);
};

#endif