#include <stdlib.h>
#include <stdio.h>

#include "textures.h"

SDL_Texture* load_texture(const char* path, SDL_Renderer* p_renderer)
{
  SDL_Texture* p_new_texture = NULL;
  SDL_Surface* p_image_surface = SDL_LoadBMP(path);

  if (p_image_surface == NULL)
  {
    printf("ERROR: Unable to load texture at: \"%s\"\n", path);
    exit(EXIT_FAILURE);
  }

  p_new_texture = SDL_CreateTextureFromSurface(p_renderer, p_image_surface);

  if (p_new_texture == NULL)
  {
    printf("ERROR: Unable to create texture from the pixels of the image at: \"%s\"\n", path);
    exit(EXIT_FAILURE);
  }

  SDL_FreeSurface(p_image_surface);

  return p_new_texture;
}
