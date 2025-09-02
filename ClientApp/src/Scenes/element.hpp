/// Copyright Los Más Fritos - 2025

#ifndef ELEMENT_HPP
#define ELEMENT_HPP

class Element {
  private:
    float x, visible_x, hide_x;
    float y, visible_y, hide_y;

    float speed;

    bool hiding;
  public:
    Element(float visible_x, float hide_x, float visible_y, float hide_y);
    ~Element();
};

#endif