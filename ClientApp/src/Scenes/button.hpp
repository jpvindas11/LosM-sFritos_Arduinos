/// Copyright Los Más Fritos - 2025

#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <SDL2/SDL.h>
#include <iostream>
#include <SDL2/SDL_ttf.h>

class Button{

    public:
        void createButton(SDL_Renderer *renderer,int x, int y, int w, int h, int r, int g, int b, int a, const char* msg, TTF_Font* font);

        bool isPressed(SDL_Event& e);

     private:
        SDL_Rect button;

};

#endif // BUTTONUI_HPP