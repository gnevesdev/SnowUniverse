#ifndef TEXTURES_H
#define TEXTURES_H

#include <SDL2/SDL.h>

SDL_Texture* load_texture(
  const char*   path,
  SDL_Renderer* p_renderer
);

#endif

