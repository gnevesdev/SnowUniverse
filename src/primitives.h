#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <SDL.h>

void fillCircle(
	SDL_Renderer* p_renderer,
	int xpos, int ypos,
	int radius,
	Uint8 r, Uint8 g, Uint8 b,
	Uint8 alpha
);

#endif
