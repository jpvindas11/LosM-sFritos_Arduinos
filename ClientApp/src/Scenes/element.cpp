/// Copyright Los Más Fritos - 2025

#include "element.hpp"

Element::Element(float visible_x, float hide_x, float visible_y, float hide_y, int w, int h) : w(w), h(h) {
    this->visible_x = visible_x;
    this->x = this->hide_x = hide_x;

    this->visible_y = visible_y;
    this->y = this->hide_y = hide_y;

    this->hiding = true;
}

Element::~Element() {}

