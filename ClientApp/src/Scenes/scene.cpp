/// Copyright Los Más Fritos - 2025

#include "scene.hpp"

Scene::Scene(SCENE_NAME name, COLOR bg_color) : name(name), background_color(bg_color) {
}

Scene::~Scene() {
    elements.clear();
    buttons.clear();
}

void Scene::add_element(Element element) {
    elements.push_back(element);
}

void Scene::add_button(Button button) {
    this->add_element(button.getElement());
    buttons.push_back(button);
}

void Scene::tick_elements() {
    for (Element& element : elements) {
        // Update element position here
    }
}

void Scene::tick_buttons(bool mouse_button_state, float mouse_x, float mouse_y) {
    for (Button& button : buttons) {
        // Check if button is hovered
        if (button.checkIfHovered(mouse_x, mouse_y)) {
            if (button.setClickedState(mouse_button_state)) {
                // Handle button actions




                
            }
        }
    }
}