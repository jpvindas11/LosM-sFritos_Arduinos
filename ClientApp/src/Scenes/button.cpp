#include <button.hpp>
#include "element.hpp"

Button::Button(float visible_x, float hide_x, float visible_y, float hide_y, int w, int h) 
: element(visible_x, hide_x, visible_y, hide_y, w, h) {
    this->clicked = false;
    this->hovered = false;
}

Button::~Button() {}

bool Button::isPressed() {
    return this->clicked;
}

bool Button::setClickedState(bool mouse_button_state) {
    this->clicked = mouse_button_state;
    return this->clicked;
}

bool Button::checkIfHovered(float mouse_x, float mouse_y) {
    if (mouse_x >= element.getX() && mouse_x <= element.getX() + element.getW() &&
        mouse_y >= element.getY() && mouse_y <= element.getY() + element.getH()) {
        this->hovered = true;
        return true;
    }
    this->hovered = false;
    return false;
}
