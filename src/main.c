#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <math.h>

#include <SDL2/SDL.h>

#include "vecmath.h"
#include "primitives.h"
#include "textures.h"

/* TODO
- Add support for game controller in the future
- Make a sprite for the spaceship
*/

#define FPS 60

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 720

#define FRAMES_BETWEEN_ORBIT_PREDICTION 2

typedef enum
{
	GAME_STATE_MENU,
	GAME_STATE_GAME
} game_state_t;

typedef enum
{
	DIRECTION_UNDEFINED,
	DIRECTION_LEFT,
	DIRECTION_RIGHT
} direction_t;

typedef struct
{
	Vector2_t position;
	uint8_t radius;
	unsigned int mass;
} space_body_t;

typedef struct
{
	Vector2_t position;
	uint8_t size;
	unsigned int mass;
	direction_t direction;
	float angle;
	Vector2_t velocity;
	Vector2_t forward;
	bool engine;
} spaceship_t;

static game_state_t game_state;

static space_body_t sun_object = { { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }, 80, 2000 };
static spaceship_t player_object = { {80.f, 80.f}, 16, 6000, (direction_t)DIRECTION_UNDEFINED, 0, {.0f, .0f}, {.0f, .0f}, false };

static SDL_Point* orbit_prediction_points;

static void handle_events(bool* p_running_condition);
static void update(SDL_Window* p_window);
static void render(SDL_Renderer* p_renderer, SDL_Texture* p_menu_texture);

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != (int)NULL)
	{
		puts("ERROR: Unable to initialize SDL!");
		exit(EXIT_FAILURE);
	}

	SDL_Window* p_game_window;
	p_game_window = SDL_CreateWindow("Snow Universe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);

	if (!p_game_window)
	{
		puts("ERROR: Could not create the game window!");
		exit(EXIT_FAILURE);
	}

	SDL_Renderer* p_game_renderer;
	p_game_renderer = SDL_CreateRenderer(p_game_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (!p_game_renderer)
	{
		puts("ERROR: Could not create a proper renderer!");
		exit(EXIT_FAILURE);
	}

	bool running = true;

	orbit_prediction_points = (SDL_Point*)malloc(1000 * sizeof(SDL_Point));

	player_object.forward.x = cos(player_object.angle);
	player_object.forward.y = sin(player_object.angle);

	game_state = (game_state_t)GAME_STATE_MENU;

	SDL_Texture* p_menu_texture = loadTexture("assets/menu.bmp", p_game_renderer);

	while (running)
	{
		static int frame_start;
		frame_start = SDL_GetTicks();

		handle_events(&running);
		update(p_game_window);
		render(p_game_renderer, p_menu_texture);

		static int delta_time;
		delta_time = SDL_GetTicks() - frame_start;

		if (delta_time < 1000 / FPS) SDL_Delay(1000 / FPS - delta_time);
	}

	SDL_DestroyTexture(p_menu_texture);
	p_menu_texture = NULL;
	SDL_DestroyRenderer(p_game_renderer);
	p_game_renderer = NULL;
	SDL_DestroyWindow(p_game_window);
	p_game_window = NULL;
	SDL_Quit();

	return 0;
}

static void handle_events(bool* p_running_condition)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			*p_running_condition = false;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_a:
				player_object.direction = (direction_t)DIRECTION_LEFT;
				break;

			case SDLK_d:
				player_object.direction = (direction_t)DIRECTION_RIGHT;
				break;
					
			case SDLK_SPACE:
				if (game_state == (game_state_t)GAME_STATE_GAME)
					player_object.engine = true;
				break;

			case SDLK_RETURN:
				if (game_state != (game_state_t)GAME_STATE_GAME)
					game_state = (game_state_t)GAME_STATE_GAME;
				break;
			}
			break;

		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_a:
				player_object.direction = (direction_t)DIRECTION_UNDEFINED;
				break;

			case SDLK_d:
				player_object.direction = (direction_t)DIRECTION_UNDEFINED;
				break;
					
			case SDLK_SPACE:
				player_object.engine = false;
				break;
			}
			break;
		}
	}

	return;
}

static void update_player_direction(void)
{
	switch (player_object.direction)
	{
	case (direction_t)DIRECTION_LEFT:
		player_object.angle -= .1f;

		if (player_object.angle < 0) player_object.angle = 2 * PI;

		player_object.forward.x = cos(player_object.angle);
		player_object.forward.y = sin(player_object.angle);

		break;

	case (direction_t)DIRECTION_RIGHT:
		player_object.angle += .1f;

		if (player_object.angle > 2 * PI) player_object.angle = 0;

		player_object.forward.x = cos(player_object.angle);
		player_object.forward.y = sin(player_object.angle);

		break;
	}

	return;
}

static Vector2_t gimme_that_bad_boy_gravity(Vector2_t body1_position, int body1_mass, Vector2_t body2_position, int body2_mass)
{
	Vector2_t difference = vector2Sub(body2_position, body1_position);
	float distance = vector2Distance(body1_position, body2_position);
	Vector2_t gravity_force = vector2Mul(vector2Normalized(difference), (Vector2_t) { GRAVITATIONAL_CONSTANT * body1_mass * body2_mass / (distance * distance), GRAVITATIONAL_CONSTANT * body1_mass * body2_mass / (distance * distance) });

	gravity_force.x /= body1_mass;
	gravity_force.y /= body1_mass;

	return gravity_force;
}

static Vector2_t do_gravity_thing(void)
{
	if (player_object.engine)
	{
		Vector2_t engine_impulse = (Vector2_t) { player_object.forward.x / 100, player_object.forward.y / 100 };

		player_object.velocity = vector2Sub(player_object.velocity, engine_impulse);
	}

	player_object.velocity = vector2Sum(player_object.velocity, gimme_that_bad_boy_gravity(player_object.position, player_object.mass, sun_object.position, sun_object.mass));
	player_object.position = vector2Sum(player_object.position, player_object.velocity);

	return player_object.velocity;
}

static void predict_orbit(Vector2_t object_position, Vector2_t object_velocity, int object_mass, Vector2_t attractor_position, int attractor_mass, int steps)
{
	object_velocity = vector2Mul(object_velocity, (Vector2_t) { 2, 2 });

	for (int i = 0; i < steps; ++i)
	{
		orbit_prediction_points[i] = (SDL_Point) { object_position.x, object_position.y };
		object_velocity = vector2Sum(object_velocity, vector2Mul(gimme_that_bad_boy_gravity(object_position, object_mass, attractor_position, attractor_mass), (Vector2_t) { 4, 4 }));
		object_position = vector2Sum(object_position, object_velocity);
	}

	return;
}

static void update_player_orbit_prediction_each_x_frames(Vector2_t player_velocity)
{
	static int timer = FRAMES_BETWEEN_ORBIT_PREDICTION;

	if (timer > 0)
	{
		--timer;
	}
	else if (timer <= 0)
	{
		predict_orbit(player_object.position, player_velocity, player_object.mass, sun_object.position, sun_object.mass, 1000);

		timer = FRAMES_BETWEEN_ORBIT_PREDICTION;
	}

	return;
}

static void update_sun_position_when_resizing(SDL_Window* p_window)
{
	int screen_width;
	int screen_height;
	
	SDL_GetWindowSize(p_window, &screen_width, &screen_height);

	sun_object.position = (Vector2_t) { screen_width / 2, screen_height / 2 };

	return;
}

static bool check_collision_with_sun(void)
{
	float distance_to_sun = vector2Distance(player_object.position, sun_object.position);

	if (distance_to_sun <= sun_object.radius)
	{
		return true;
	}

	return false;
}

static bool is_out_of_screen(SDL_Window* p_window, Vector2_t position)
{
	int screen_width;
	int screen_height;
	
	SDL_GetWindowSize(p_window, &screen_width, &screen_height);

	if (position.x < 0 || position.x > screen_width || position.y < 0 || position.y > screen_height)
	{
		return true;
	}

	return false;
}

static void player_die(void)
{
	game_state = (game_state_t)GAME_STATE_MENU;
		
	player_object.position = (Vector2_t) { 80.f, 80.f };
	player_object.velocity = (Vector2_t) { 0.f, 0.f };

	return;
}

static void update(SDL_Window* p_window)
{
	if (game_state != (game_state_t)GAME_STATE_GAME) return;

	update_sun_position_when_resizing(p_window);
	update_player_direction();
	update_player_orbit_prediction_each_x_frames(do_gravity_thing());
	
	if (check_collision_with_sun() || is_out_of_screen(p_window, player_object.position))
	{
		player_die();
	}

	return;
}

static void render_planets(SDL_Renderer* p_renderer)
{
	fillCircle(p_renderer, (int)sun_object.position.x, (int)sun_object.position.y, sun_object.radius, 255, 215, 100, 255);

	return;
}

static void render_player(SDL_Renderer* p_renderer)
{
	SDL_Rect player = (SDL_Rect) { (int)player_object.position.x - player_object.size / 2, (int)player_object.position.y - player_object.size / 2, player_object.size, player_object.size };

	SDL_SetRenderDrawColor(p_renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(p_renderer, &player);

	SDL_SetRenderDrawColor(p_renderer, 0, 0, 255, 255);
	SDL_RenderDrawLine(p_renderer, player_object.position.x, player_object.position.y, player_object.position.x + player_object.forward.x * 15, player_object.position.y + player_object.forward.y * 15);

	return;
}

static void draw_orbit_prediction(SDL_Renderer* p_renderer)
{
	SDL_SetRenderDrawColor(p_renderer, 0, 255, 0, 255);
	SDL_RenderDrawLines(p_renderer, orbit_prediction_points, 1000);

	return;
}

static void render_menu(SDL_Renderer* p_renderer, SDL_Texture* p_menu_texture)
{
	static int timer;
	bool draw_phrase = false;

	if (timer < 30)
	{
		++timer;
	}
	else
	{
		draw_phrase = !draw_phrase;
	}

	if (!draw_phrase) return;

	SDL_SetRenderDrawColor(p_renderer, 255, 255, 255, 255);
	SDL_RenderCopy(p_renderer, p_menu_texture, NULL, NULL);

	return;
}

static void render(SDL_Renderer* p_renderer, SDL_Texture* p_menu_texture)
{
	SDL_SetRenderDrawColor(p_renderer, 0, 0, 0, 255);
	SDL_RenderClear(p_renderer);

	if (game_state != (game_state_t)GAME_STATE_GAME)
	{
		render_menu(p_renderer, p_menu_texture);
		goto PRESENT_GRAPHICS;
	}

	render_planets(p_renderer);
	draw_orbit_prediction(p_renderer);
	render_player(p_renderer);

	PRESENT_GRAPHICS:
	SDL_RenderPresent(p_renderer);

	return;
}
