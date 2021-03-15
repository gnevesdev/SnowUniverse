#include "textures.h"

#include <stdlib.h>
#include <stdio.h>

SDL_Texture* loadTexture(const char* path, SDL_Renderer* p_renderer)
{
  SDL_Texture* p_newTexture = NULL;

  SDL_Surface* p_imageSurface = SDL_LoadBMP(path);

  if (p_imageSurface == NULL)
  {
    printf("ERROR: Unable to load texture at: %s\n", path);
    exit(EXIT_FAILURE);
  }

  p_newTexture = SDL_CreateTextureFromSurface(p_renderer, p_imageSurface);

  if (p_newTexture == NULL)
  {
    printf("ERROR: Unable to create texture from the pixels of the image at: %s\n", path);
    exit(EXIT_FAILURE);
  }

  SDL_FreeSurface(p_imageSurface);

  return p_newTexture;
}
