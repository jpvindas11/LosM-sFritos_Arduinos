/// Copyright Los Más Fritos - 2025

#ifndef COLORDEF_HPP
#define COLORDEF_HPP

#define RGB_SLOTS 3

typedef enum COLOR {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_AMOUNT,
} COLOR;

extern int colors_rgb[COLOR_AMOUNT][RGB_SLOTS];

#endif