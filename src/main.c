#include <stdio.h>
#include <SDL.h>
#include <stdlib.h>
#include <stdbool.h>
#include "vecmath.h"
#include "primitives.h"
#include <math.h>
#include "textures.h"

/* TODO
- Add support for game controller in the future
- Try adding glow effect to the sun
- Make a main menu
- Make a sprite for that fucking square spaceship
*/

typedef enum GameState
{
	MENU,
	GAME
} GameState_t;

typedef enum Direction
{
	NULLDIR, LEFT, RIGHT
} Direction_t;

typedef struct Planet
{
	Vector2_t position;
	Uint8 radius;
	Uint16 mass;
} Planet_t;

typedef struct Spaceship
{
	Vector2_t position;
	Uint8 size;
	Uint16 mass;
	Vector2_t forward;
	float angle;
	Direction_t direction;
	bool engine;
	Vector2_t velocity;
} Spaceship_t;

#define FPS 60

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 720

#define FRAMES_BETWEEN_ORBIT_PREDICTION 2

static GameState_t gameState;

static Planet_t mainPlanetObject = {
	{SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
	80,
	2000
};

static Spaceship_t playerObject = {
	{80.f, 80.f},
	16,
	6000,
	{.0f, .0f},
	0,
	(Direction_t)NULLDIR,
	false
};

static SDL_Point* possibleOrbitPoints;

static void handleEvents(bool* p_runningCondition);
static void update(SDL_Window* p_window);
static void render(SDL_Renderer* p_renderer, SDL_Texture* p_menuTexture);

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
	{
		printf("ERROR: Unable to initialize SDL!\n");
		exit(EXIT_FAILURE);
	}

	SDL_Window* p_gameWindow;

	p_gameWindow = SDL_CreateWindow(
		"Snow Universe",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		/*(int)NULL*/ SDL_WINDOW_RESIZABLE
	);

	if (!p_gameWindow)
	{
		printf("ERROR: Could not create the game window!\n");
		exit(EXIT_FAILURE);
	}

	SDL_Renderer* p_gameRenderer;

	p_gameRenderer = SDL_CreateRenderer(
		p_gameWindow,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!p_gameRenderer)
	{
		printf("ERROR: Could not create a proper renderer!\n");
		exit(EXIT_FAILURE);
	}

	bool running = true;

	possibleOrbitPoints = (SDL_Point*)malloc(1000 * sizeof(SDL_Point));

	playerObject.forward.x = cos(playerObject.angle);
	playerObject.forward.y = sin(playerObject.angle);

	gameState = (GameState_t)MENU;

	SDL_Texture* p_menuTexture = loadTexture("assets/menu.bmp", p_gameRenderer);

	while (running)
	{
		static Uint32 frameStart;
		frameStart = SDL_GetTicks();

		handleEvents(&running);
		update(p_gameWindow);
		render(p_gameRenderer, p_menuTexture);

		static Uint32 deltaTime;
		deltaTime = SDL_GetTicks() - frameStart;

		if (deltaTime < 1000 / FPS)
			SDL_Delay(1000 / FPS - deltaTime);
	}

	SDL_DestroyTexture(p_menuTexture);
	p_menuTexture = NULL;
	SDL_DestroyRenderer(p_gameRenderer);
	p_gameRenderer = NULL;
	SDL_DestroyWindow(p_gameWindow);
	p_gameWindow = NULL;
	SDL_Quit();

	return 0;
}

static void handleEvents(bool* p_runningCondition)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			*p_runningCondition = false;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_a:
				playerObject.direction = (Direction_t)LEFT;
				break;

			case SDLK_d:
				playerObject.direction = (Direction_t)RIGHT;
				break;
					
			case SDLK_SPACE:
				if (gameState == (GameState_t)GAME)
					playerObject.engine = true;
				break;

			case SDLK_RETURN:
				if (gameState != (GameState_t)GAME)
					gameState = (GameState_t)GAME;
				break;
			}
			break;

		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
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

	return;
}

static void updatePlayerDirection(void)
{
	switch (playerObject.direction)
	{
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

	return;
}

static Vector2_t gimmeThatBadBoyGravity(
	Vector2_t body1position,
	Uint16 body1mass,
	Vector2_t body2position,
	Uint16 body2mass
)
{
	Vector2_t difference = vector2Sub(
		body2position,
		body1position
	);

	float distance = vector2Distance(
		body1position,
		body2position
	);

	Vector2_t gravityForce = vector2Mul(
		vector2Normalized(difference),
		(Vector2_t) {
			GRAVITATIONAL_CONSTANT * body1mass * body2mass / (distance * distance),
			GRAVITATIONAL_CONSTANT * body1mass * body2mass / (distance * distance)
		}
	);

	gravityForce.x /= body1mass;
	gravityForce.y /= body1mass;

	return gravityForce;
}

static Vector2_t doFuckingGravity(void)
{
	if (playerObject.engine)
	{
		Vector2_t engineImpulse = (Vector2_t) {
			playerObject.forward.x / 100,
			playerObject.forward.y / 100
		};

		playerObject.velocity = vector2Sub(
			playerObject.velocity,
			engineImpulse
		);
	}

	Planet_t closestPlanet = mainPlanetObject;

	playerObject.velocity = vector2Sum(
		playerObject.velocity,
		gimmeThatBadBoyGravity(
			playerObject.position,
			playerObject.mass,
			closestPlanet.position,
			closestPlanet.mass
		)
	);

	playerObject.position = vector2Sum(
		playerObject.position,
		playerObject.velocity
	);

	return playerObject.velocity;
}

static void predictOrbit(
	Vector2_t objectPosition,
	Vector2_t objectVelocity,
	Uint16 objectMass,
	Vector2_t attractorPosition,
	Uint16 attractorMass,
	Uint16 steps
)
{
	objectVelocity = vector2Mul(
		objectVelocity,
		(Vector2_t) {2, 2}
	);

	for (Uint16 i = 0; i < steps; ++i)
	{
		possibleOrbitPoints[i] = (SDL_Point) {
			objectPosition.x,
			objectPosition.y
		};

		objectVelocity = vector2Sum(
			objectVelocity,
			vector2Mul(
				gimmeThatBadBoyGravity(
					objectPosition,
					objectMass,
					attractorPosition,
					attractorMass
				),
				(Vector2_t) {4, 4}
			)
		);

		objectPosition = vector2Sum(
			objectPosition,
			objectVelocity
		);
	}

	return;
}

static void updatePlayerOrbitPredictionEachXFrames(
	Vector2_t playerVelocity
)
{
	static Uint8 timer = FRAMES_BETWEEN_ORBIT_PREDICTION;

	if (timer > 0)
	{
		--timer;
	}
	else if (timer <= 0)
	{
		predictOrbit(
			playerObject.position,
			playerVelocity,
			playerObject.mass,
			mainPlanetObject.position,
			mainPlanetObject.mass,
			1000
		);

		timer = FRAMES_BETWEEN_ORBIT_PREDICTION;
	}

	return;
}

static void updateSunPositionWhenResizing(SDL_Window* p_window)
{
	Vector2_t screenArea;
	int screenWidth;
	int screenHeight;
	
	SDL_GetWindowSize(
		p_window,
		&screenWidth,
		&screenHeight
	);

	mainPlanetObject.position = (Vector2_t) {
		screenWidth / 2,
		screenHeight / 2
	};
}

bool checkCollisionWithSun(void)
{
	float distanceToSun = vector2Distance(
		playerObject.position,
		mainPlanetObject.position
	);

	if (distanceToSun <= mainPlanetObject.radius)
	{
		return true;
	}

	return false;
}

static void update(SDL_Window* p_window)
{
	if (gameState != (GameState_t)GAME) return;

	updateSunPositionWhenResizing(p_window);
	updatePlayerDirection();
	updatePlayerOrbitPredictionEachXFrames(
		doFuckingGravity()
	);
	
	if (checkCollisionWithSun())
	{
		gameState = (GameState_t)MENU;
		
		playerObject.position = (Vector2_t) {80.f, 80.f};
		playerObject.velocity = (Vector2_t) {0.f, 0.f};
	}

	return;
}

static void renderPlanets(SDL_Renderer* p_renderer)
{
	fillCircle(
		p_renderer,
		(int)mainPlanetObject.position.x,
		(int)mainPlanetObject.position.y,
		mainPlanetObject.radius,
		255, 215, 100,
		255
	);

	return;
}

static void renderPlayer(SDL_Renderer* p_renderer)
{
	SDL_Rect player = (SDL_Rect) {
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

	return;
}

static void drawOrbitPrediction(SDL_Renderer* p_renderer)
{
	SDL_SetRenderDrawColor(p_renderer, 0, 255, 0, 255);
	SDL_RenderDrawLines(p_renderer, possibleOrbitPoints, 1000);

	return;
}

static void renderMenu(SDL_Renderer* p_renderer, SDL_Texture* p_menuTexture)
{
	static Uint8 timer;
	bool drawPhrase = false;

	if (timer < 30)
	{
		++timer;
	}
	else
	{
		drawPhrase = !drawPhrase;
	}

	if (!drawPhrase) return;

	SDL_SetRenderDrawColor(p_renderer, 255, 255, 255, 255);
	SDL_RenderCopy(p_renderer, p_menuTexture, NULL, NULL);

	return;
}

static void render(
	SDL_Renderer* p_renderer,
	SDL_Texture* p_menuTexture
)
{
	SDL_SetRenderDrawColor(p_renderer, 0, 0, 0, 255);
	SDL_RenderClear(p_renderer);

	if (gameState != (GameState_t)GAME)
	{
		renderMenu(p_renderer, p_menuTexture);
		goto presentGraphics;
	}

	renderPlanets(p_renderer);
	renderPlayer(p_renderer);
	drawOrbitPrediction(p_renderer);

	presentGraphics:
	SDL_RenderPresent(p_renderer);

	return;
}
