#include <stdio.h>
#include <SDL.h>
#include <stdlib.h>
#include <stdbool.h>
#include "vecmath.h"
#include "primitives.h"

/* TODO
- Add support for game controller in the future
*/

typedef enum Direction {
	NULLDIR, LEFT, RIGHT
} Direction_t;

typedef struct Planet {
	Vector2_t position;
	Uint8 radius;
	Uint8 mass;
} Planet_t;

typedef struct Spaceship {
	Vector2_t position;
	Uint8 size;
	Uint8 mass;
	Vector2_t forward;
	float angle;
	Direction_t direction;
	bool engine;
} Spaceship_t;

#define FPS 60

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

static Planet_t mainPlanetObject = {
	{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
	80,
	300
};

static Spaceship_t playerObject = {
	{80.f, 80.f},
	16,
	100,
	{.0f, .0f},
	0,
	(Direction_t)NULLDIR,
	false
};

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

	playerObject.forward.x = cos(playerObject.angle);
	playerObject.forward.y = sin(playerObject.angle);

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

				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
					case SDLK_a:
						playerObject.direction = (Direction_t)LEFT;
						break;

					case SDLK_d:
						playerObject.direction = (Direction_t)RIGHT;
						break;
					
					case SDLK_SPACE:
						playerObject.engine = true;
						break;
					}
					break;

				case SDL_KEYUP:
					switch (event.key.keysym.sym) {
					case SDLK_a:
						playerObject.direction = (Direction_t)NULLDIR;
						break;

					case SDLK_d:
						playerObject.direction = (Direction_t)NULLDIR;
						break;
					
					case SDLK_SPACE:
						playerObject.engine = false;
						break;
					}
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
	/* Player direction */ {
		switch (playerObject.direction) {
		case (Direction_t)LEFT:
			playerObject.angle -= .1f;

			if (playerObject.angle < 0)
				playerObject.angle = 2 * PI;

			playerObject.forward.x = cos(playerObject.angle);
			playerObject.forward.y = sin(playerObject.angle);

			break;

		case (Direction_t)RIGHT:
			playerObject.angle += .1f;

			if (playerObject.angle > 2 * PI)
				playerObject.angle = 0;

			playerObject.forward.x = cos(playerObject.angle);
			playerObject.forward.y = sin(playerObject.angle);

			break;
		}
	}

	/* FUCKING GRAVITY!!!!!! */ {
		Vector2_t velocity = vector2Difference(
			playerObject.position,
			mainPlanetObject.position
		);

		if (playerObject.engine) {
			velocity.x += playerObject.forward.x * 1000;
			velocity.y += playerObject.forward.y * 1000;
		}

		velocity = vector2Normalized(velocity);

		float distanceToClosestPlanet = vector2Distance(
			playerObject.position,
			mainPlanetObject.position
		);

		float gravityForce = gimmeThatBadBoyGravity(
			GRAVITATIONAL_CONSTANT,
			playerObject.mass,
			mainPlanetObject.mass,
			distanceToClosestPlanet
		);

		if ((int)distanceToClosestPlanet != 0) {
			playerObject.position.x -= velocity.x * gravityForce;
			playerObject.position.y -= velocity.y * gravityForce;
		}
	}

	return;
}

void render(SDL_Renderer* p_renderer) {
	SDL_SetRenderDrawColor(p_renderer, 0, 0, 0, 255);
	SDL_RenderClear(p_renderer);

	/* Square Planet */ {
		fillCircle(
			p_renderer,
			(int)mainPlanetObject.position.x,
			(int)mainPlanetObject.position.y,
			mainPlanetObject.radius,
			45, 62, 30,
			255
		);
	}

	/* Player */ {
		SDL_Rect player = (SDL_Rect){
			(int)playerObject.position.x - playerObject.size / 2,
			(int)playerObject.position.y - playerObject.size / 2,
			playerObject.size,
			playerObject.size
		};

		SDL_SetRenderDrawColor(p_renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(p_renderer, &player);

		SDL_SetRenderDrawColor(p_renderer, 0, 0, 255, 255);
		SDL_RenderDrawLine(
			p_renderer,
			playerObject.position.x,
			playerObject.position.y,
			playerObject.position.x + playerObject.forward.x * 15,
			playerObject.position.y + playerObject.forward.y * 15
		);
	}

	SDL_RenderPresent(p_renderer);

	return;
}
