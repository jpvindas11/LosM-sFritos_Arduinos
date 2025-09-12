/// Copyright Los Más Fritos - 2025

#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <iostream>

class Element;

typedef enum BUTTON_ACTION {
    BUTTON_ACTION_NONE,
    BUTTON_ACTION_CHANGE_SCENE,
    BUTTON_ACTION_OPEN_SLIDER,
    BUTTON_ACTION_TOGGLE,
    BUTTON_ACTION_EXIT,
    BUTTON_ACTION_ACCEPT,
    BUTTON_ACTION_CANCEL,
} BUTTON_ACTION;

class Button{

    public:
        //void createButton(SDL_Renderer *renderer,int x, int y, int w, int h, int r, int g, int b, int a, const char* msg, TTF_Font* font);

        Button(float visible_x, float hide_x, float visible_y, float hide_y, int w, int h);
        ~Button();

        bool isPressed();
        bool setClickedState(bool mouse_button_state);
        bool checkIfHovered(float mouse_x, float mouse_y);

        Element getElement() { return this->element; }

     private:
        Element element;

        bool clicked;
        bool hovered;

};

#endif // BUTTONUI_HPP