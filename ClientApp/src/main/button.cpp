#include <button.hpp>
#include <SDL2/SDL.h>



void Button::createButton(SDL_Renderer *renderer, int x, int y, int w, int h, int r, int g, int b, int a, const char* msg, TTF_Font* font){
    button = { x, y, w, h };
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, &button);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &button);

    SDL_Color textColor = { 255, 255, 255, 255 };  // blanco
    SDL_Surface* surface = TTF_RenderText_Solid(font, msg, textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, surface);

    int tw, th;
    SDL_QueryTexture(textTexture, NULL, NULL, &tw, &th);

    SDL_Rect textRect;
    textRect.x = x + (w - tw) / 2;
    textRect.y = y + (h - th) / 2;
    textRect.w = tw;
    textRect.h = th;

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(textTexture);
}

bool Button::isPressed(SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mousex = e.button.x;
        int mousey = e.button.y;

        if (mousex >= button.x && mousex <= button.x + button.w &&
            mousey >= button.y && mousey <= button.y + button.h) {
            return true;
        }
    }
    return false;
}

