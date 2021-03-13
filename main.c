#include <stdio.h>
#include <SDL.h>
#include <stdlib.h>
#include <stdbool.h>

/* TODO
- Add support for game controller in the future
*/

int main(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		printf("ERROR: Unable to initialize SDL!\n");
		exit(EXIT_FAILURE);
	}
	
	SDL_Window* gameWindow = SDL_CreateWindow(
		"Snow Universe",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		800,
		600,
		NULL
	);

	if (!gameWindow) {
		printf("ERROR: Could not create the game window!\n");
		exit(EXIT_FAILURE);
	}

	SDL_Renderer* gameRenderer = SDL_CreateRenderer(
		gameWindow,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!gameRenderer) {
		printf("ERROR: Could not create a proper renderer!\n");
		exit(EXIT_FAILURE);
	}

	bool running = true;

	while (running) {
		/* EVENTS SCOPE */ {
			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_QUIT:
					running = false;
					break;
				}
			}
		}
	}

	SDL_DestroyRenderer(gameRenderer);
	gameRenderer = NULL;
	SDL_DestroyWindow(gameWindow);
	gameWindow = NULL;
	SDL_Quit();

	return 0;
}
