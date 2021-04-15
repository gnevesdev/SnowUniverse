#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <stdint.h>

#include <SDL2/SDL.h>

void fill_circle(SDL_Renderer* p_renderer, int x_pos, int y_pos, int radius, uint8_t r, uint8_t g, uint8_t b, uint8_t alpha);

#endif
