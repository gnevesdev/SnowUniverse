#include "primitives.h"

/*
I stole this function nicely from:
https://gist.github.com/derofim/912cfc9161269336f722
:P
*/
void fillCircle(
	SDL_Renderer* p_renderer,
	int xpos, int ypos,
	int radius,
	Uint8 r, Uint8 g, Uint8 b,
	Uint8 alpha
) {
	static const int BPP = 4;

	for (double dy = 1; dy <= radius; dy += 1.0) {
		double dx = floor(sqrt((2.0 * radius * dy) - (dy * dy)));
		int x = xpos - dx;

		SDL_SetRenderDrawColor(p_renderer, r, g, b, alpha);
		
		SDL_RenderDrawLine(
			p_renderer,
			xpos - dx,
			ypos + dy - radius,
			xpos + dx,
			ypos + dy - radius
		);

		SDL_RenderDrawLine(
			p_renderer,
			xpos - dx,
			ypos - dy + radius,
			xpos + dx,
			ypos - dy + radius
		);
	}
}
