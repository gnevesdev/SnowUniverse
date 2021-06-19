#include <math.h>

#include "primitives.h"

/* I stole this function from:                             */
/* https://gist.github.com/derofim/912cfc9161269336f722 :P */
void fill_circle(
  SDL_Renderer* p_renderer,
  int           x_pos,
  int           y_pos,
  int           radius,
  uint8_t       r,
  uint8_t       g,
  uint8_t       b,
  uint8_t       alpha
)
{
  for (int dy = 1; dy <= radius; ++dy)
  {
    int dx = floor(sqrt((2.0 * radius * dy) - (dy * dy)));
    int x = x_pos - dx;

    SDL_SetRenderDrawColor(p_renderer, r, g, b, alpha);
		
    SDL_RenderDrawLine(
      p_renderer,
      x_pos - dx,
      y_pos + dy - radius,
      x_pos + dx,
      y_pos + dy - radius
    );

    SDL_RenderDrawLine(
      p_renderer,
      x_pos - dx,
      y_pos - dy + radius,
      x_pos + dx,
      y_pos - dy + radius
    );
  }
}

