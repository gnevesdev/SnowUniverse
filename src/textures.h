#ifndef TEXTURES_H
#define TEXTURES_H

#include <SDL.h>

SDL_Texture* loadTexture(
  const char* path,
  SDL_Renderer* p_renderer
);

#endif