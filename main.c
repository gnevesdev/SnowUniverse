#include <stdio.h>
#include <SDL.h>
#include <stdlib.h>
#include <stdbool.h>
#include "math.h"

/* TODO
- Add support for game controller in the future
*/

typedef struct Planet {
	Vector2_t position;
	Uint8 size;
} Planet_t;

typedef struct Spaceship {
	Vector2_t position;
	Uint8 size;
} Spaceship_t;

#define FPS 60

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

static Planet_t mainPlanetObject = {
	{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
	80
};

static Spaceship_t playerObject = {{80, 80}, 16};

void update(void);
void render(SDL_Renderer* p_renderer);

int main(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		printf("ERROR: Unable to initialize SDL!\n");
		exit(EXIT_FAILURE);
	}

	static SDL_Window* p_gameWindow;
	
	p_gameWindow = SDL_CreateWindow(
		"Snow Universe",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		NULL
	);

	if (!p_gameWindow) {
		printf("ERROR: Could not create the game window!\n");
		exit(EXIT_FAILURE);
	}

	SDL_Renderer* p_gameRenderer;

	p_gameRenderer = SDL_CreateRenderer(
		p_gameWindow,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!p_gameRenderer) {
		printf("ERROR: Could not create a proper renderer!\n");
		exit(EXIT_FAILURE);
	}

	bool running = true;

	while (running) {
		static Uint32 frameStart;
		frameStart = SDL_GetTicks();

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

		update();
		render(p_gameRenderer);

		static Uint32 deltaTime;
		deltaTime = SDL_GetTicks() - frameStart;

		if (deltaTime < 1000 / FPS)
			SDL_Delay(1000 / FPS - deltaTime);
	}

	SDL_DestroyRenderer(p_gameRenderer);
	p_gameRenderer = NULL;
	SDL_DestroyWindow(p_gameWindow);
	p_gameWindow = NULL;
	SDL_Quit();

	return 0;
}

void update(void) {
	if (playerObject.position.x < mainPlanetObject.position.x)
		++playerObject.position.x;
	else if (playerObject.position.x > mainPlanetObject.position.x)
		--playerObject.position.x;

	if (playerObject.position.y < mainPlanetObject.position.y)
		++playerObject.position.y;
	else if (playerObject.position.y > mainPlanetObject.position.y)
		--playerObject.position.y;

	return;
}

void render(SDL_Renderer* p_renderer) {
	SDL_SetRenderDrawColor(p_renderer, 0, 0, 0, 255);
	SDL_RenderClear(p_renderer);

	/* Square Planet */ {
		SDL_Rect mainPlanet = (SDL_Rect){
			mainPlanetObject.position.x - mainPlanetObject.size / 2,
			mainPlanetObject.position.y - mainPlanetObject.size / 2,
			mainPlanetObject.size,
			mainPlanetObject.size
		};

		SDL_SetRenderDrawColor(p_renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(p_renderer, &mainPlanet);
	}

	/* Player */ {
		SDL_Rect player = (SDL_Rect){
			playerObject.position.x - playerObject.size / 2,
			playerObject.position.y - playerObject.size / 2,
			playerObject.size,
			playerObject.size
		};

		SDL_SetRenderDrawColor(p_renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(p_renderer, &player);
	}

	SDL_RenderPresent(p_renderer);

	return;
}
