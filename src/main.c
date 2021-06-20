/* ------------------------------------------------------------------- */
/*  _____                       _   _       _                          */
/* /  ___|                     | | | |     (_)                         */
/* \ `--. _ __   _____      __ | | | |_ __  ___   _____ _ __ ___  ___  */
/*  `--. \ '_ \ / _ \ \ /\ / / | | | | '_ \| \ \ / / _ \ '__/ __|/ _ \ */
/* /\__/ / | | | (_) \ V  V /  | |_| | | | | |\ V /  __/ |  \__ \  __/ */
/* \____/|_| |_|\___/ \_/\_/    \___/|_| |_|_| \_/ \___|_|  |___/\___| */
/* ------------------------------------------------------------------- */

/* Info ------------ */
/* Author: gnevesdev */
/* Year: 2021        */

/* Standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <math.h>

#include <SDL2/SDL.h>

#include "vecmath.h"
#include "primitives.h"
#include "textures.h"

/* Constant definitions */
#define FPS 60

#define SCREEN_WIDTH  1200
#define SCREEN_HEIGHT 720

#define FRAMES_BETWEEN_ORBIT_PREDICTION 2

/* Enum definitions */
typedef enum
{
  GAME_STATE_MENU,
  GAME_STATE_GAME
} e_GameState_t;

typedef enum
{
  DIRECTION_UNDEFINED,
  DIRECTION_LEFT,
  DIRECTION_RIGHT
} e_Direction_t;

/* Struct definitions */
typedef struct
{
  Vector2_t    position;
  uint8_t      radius;
  unsigned int mass;
} SpaceBody_t;

typedef struct
{
  Vector2_t     position;
  uint8_t       size;
  unsigned int  mass;
  e_Direction_t direction;
  float         angle;
  Vector2_t     velocity;
  Vector2_t     forward;
  bool          engine;
} Spaceship_t;

/* Game variables */
static e_GameState_t game_state;

static SpaceBody_t sun_object = {
  (Vector2_t) { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }, // position
  80,                                                  // radius
  2000                                                 // mass
};

static Spaceship_t player_object = {
  (Vector2_t) {80.f, 80.f},           // position
  16,                                 // size
  6000,                               // mass
  (e_Direction_t)DIRECTION_UNDEFINED, // direction
  .0f,                                // angle
  (Vector2_t) {.0f, .0f},             // velocity
  (Vector2_t) {.0f, .0f},             // forward
  false                               // engine
};

static SDL_Point* orbit_prediction_points; // Array storing orbit predictions

/* Basic loop procedures */
static void handle_events(bool* p_running_condition);
static void update       (SDL_Window* p_window);
static void render       (SDL_Renderer* p_renderer, SDL_Texture* p_menu_texture);

int main(int argc, char* argv[])
{
  /* Checking if SDL was initialized correctly */
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
  {
    puts("ERROR: Unable to initialize SDL!");
    exit(EXIT_FAILURE);
  }

  /* Creating the application window */
  SDL_Window* p_game_window;
  p_game_window = SDL_CreateWindow(
    "Snow Universe",        // title
    SDL_WINDOWPOS_CENTERED, // x position
    SDL_WINDOWPOS_CENTERED, // y position
    SCREEN_WIDTH,           // width
    SCREEN_HEIGHT,          // height
    SDL_WINDOW_RESIZABLE    // flags
  );

  /* Window error handling */
  if (!p_game_window)
  {
    puts("ERROR: Could not create the game window!");
    exit(EXIT_FAILURE);
  }

  /* Creating a renderer */
  SDL_Renderer* p_game_renderer;
  p_game_renderer = SDL_CreateRenderer(
    p_game_window,                                       // parent window
    -1,                                                  // index
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC // flags
  );

  /* Error handling for the renderer */
  if (!p_game_renderer)
  {
    puts("ERROR: Could not create a proper renderer!");
    exit(EXIT_FAILURE);
  }

  bool running = true; // Set the game to run

  /* Allocating memory for all the orbit prediction points */
  orbit_prediction_points = (SDL_Point*)malloc(1000 * sizeof(SDL_Point));

  /* Calculating forward position */
  player_object.forward.x = cos(player_object.angle);
  player_object.forward.y = sin(player_object.angle);

  /* Opening the menu */
  game_state = (e_GameState_t)GAME_STATE_MENU;

  /* Getting the menu's texture from the assets */
  /* Note: error handling is already made by the procedure */
  SDL_Texture* p_menu_texture = load_texture(
    "assets/menu.bmp",
    p_game_renderer
  );

  /* Main loop */
  while (running)
  {
    /* Storing the time when the frame started */
    static int frame_start;
    frame_start = SDL_GetTicks();

    /* Call all the main loop procedures and subprocedures */
    handle_events(&running);                 // get window events and choose what to do with them
    update(p_game_window);                   // Update all the variables
    render(p_game_renderer, p_menu_texture); // Show the updates on the screen

    /* Get the time when the frame ended */
    static int delta_time;
    delta_time = SDL_GetTicks() - frame_start;

    /* Calculate the remaining time to complete the frame rate */
    /* and delaying the execution if necessary                 */
    if (delta_time < 1000 / FPS)
      SDL_Delay(1000 / FPS - delta_time);
  }

  /* Freeing memory */
  free(orbit_prediction_points);
  orbit_prediction_points = NULL;

  SDL_DestroyTexture(p_menu_texture);
  p_menu_texture = NULL;
  SDL_DestroyRenderer(p_game_renderer);
  p_game_renderer = NULL;
  SDL_DestroyWindow(p_game_window);
  p_game_window = NULL;
  SDL_Quit();

  /* Exit program with success error code */
  return EXIT_SUCCESS;
}

static void handle_events(bool* p_running_condition)
{
  SDL_Event event;
  while (SDL_PollEvent(&event)) // While there are events in queue
  {
    switch (event.type)
    {
    case SDL_QUIT: // Window close button clicked
      *p_running_condition = false;
      break;

    case SDL_KEYDOWN: // In case a key is down
      switch (event.key.keysym.sym)
      {
        /* Escape will exit the game for compatibility */
        /* with the window managers that don't have    */
        /* window close buttons                        */
      case SDLK_ESCAPE:
        *p_running_condition = false;
        break;

        /* Movement keys */
      case SDLK_a:
        player_object.direction = (e_Direction_t)DIRECTION_LEFT;
        break;

      case SDLK_d:
        player_object.direction = (e_Direction_t)DIRECTION_RIGHT;
        break;
					
        /* Activate the spaceship engine or not */
      case SDLK_SPACE:
        if (game_state == (e_GameState_t)GAME_STATE_GAME)
          player_object.engine = true;
        break;

        /* Key for returning to the menu */
      case SDLK_RETURN:
        if (game_state != (e_GameState_t)GAME_STATE_GAME)
          game_state = (e_GameState_t)GAME_STATE_GAME;
        break;
      }
      break;

    case SDL_KEYUP: // In case a key is not pressed
      switch (event.key.keysym.sym)
      {
        /* Movement keys are disabled */
      case SDLK_a:
        player_object.direction = (e_Direction_t)DIRECTION_UNDEFINED;
        break;

      case SDLK_d:
        player_object.direction = (e_Direction_t)DIRECTION_UNDEFINED;
        break;
					
        /* Disabling the engine when the key is not pressed */
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
  /* Checking what the direction variable is set to */
  switch (player_object.direction)
  {
  case (e_Direction_t)DIRECTION_LEFT:
    player_object.angle -= .1f;

    /* Angle is stored in radian values, which go */
    /* from 0 to 2*PI                             */
    if (player_object.angle < 0)
      player_object.angle = 2 * PI;

    /* Using rotation matrix to rotate in 2D space because           */
    /* quaternions would not apply here                              */
    /* Please refer to https://en.wikipedia.org/wiki/Rotation_matrix */
    /* for more information                                          */
    player_object.forward.x = cos(player_object.angle);
    player_object.forward.y = sin(player_object.angle);

    break;

  /* Same as above but for the other direction */
  case (e_Direction_t)DIRECTION_RIGHT:
    player_object.angle += .1f;

    if (player_object.angle > 2 * PI) player_object.angle = 0;

    player_object.forward.x = cos(player_object.angle);
    player_object.forward.y = sin(player_object.angle);

    break;
  }

  return;
}

static Vector2_t get_gravity(
  Vector2_t body1_position,
  int       body1_mass,
  Vector2_t body2_position,
  int       body2_mass
)
{
  /* Getting the vector of the difference from the two positions */
  Vector2_t difference = vector2_sub(body2_position, body1_position);

  /* Getting the distance between the two bodies */
  float distance = vector2_distance(body1_position, body2_position);

  /* Getting the gravity force with the Newton formula                              */
  /* Refer to https://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation */
  /* for more information                                                           */
  Vector2_t gravity_force = vector2_mul(
    vector2_normalized(difference),
    (Vector2_t) {
      GRAVITATIONAL_CONSTANT * body1_mass * body2_mass / (distance * distance),
      GRAVITATIONAL_CONSTANT * body1_mass * body2_mass / (distance * distance)
    }
  );

  /* Dividing the force by the body mass */
  gravity_force.x /= body1_mass;
  gravity_force.y /= body1_mass;

  return gravity_force;
}

static Vector2_t apply_gravity(void)
{
  if (player_object.engine)
  {
    /* Calculating the engine impulse */
    Vector2_t engine_impulse = (Vector2_t) {
      player_object.forward.x / 100,
      player_object.forward.y / 100
    };

    /* Applying the impulse to the velocity */
    player_object.velocity = vector2_sub(player_object.velocity, engine_impulse);
  }

  /* Adding the gravity to the velocity */
  player_object.velocity = vector2_sum(
    player_object.velocity,
    get_gravity(
      player_object.position,
      player_object.mass,
      sun_object.position,
      sun_object.mass
    )
  );

  /* Modifying the position depending on the velocity */
  player_object.position = vector2_sum(player_object.position, player_object.velocity);

  return player_object.velocity;
}

static void predict_orbit(
  Vector2_t object_position,
  Vector2_t object_velocity,
  int       object_mass,
  Vector2_t attractor_position,
  int       attractor_mass,
  int       steps
)
{
  /* Doubling the velocity */
  object_velocity = vector2_mul(object_velocity, (Vector2_t) { 2, 2 });

  for (size_t i = 0; i < steps; ++i)
  {
    /* Calculating the current point */
    orbit_prediction_points[i] = (SDL_Point) {
      object_position.x,
      object_position.y
    };

    /* Running physics in the future */
    object_velocity = vector2_sum(
      object_velocity,
      vector2_mul(
        get_gravity(
          object_position,
          object_mass,
          attractor_position,
          attractor_mass
        ),
        (Vector2_t) { 4, 4 }
      )
    );

    object_position = vector2_sum(object_position, object_velocity);
  }

  return;
}

static void update_player_orbit_prediction_each_x_frames(Vector2_t player_velocity)
{
  /* Initializing a timer to count the times we should do prediciton */
  static int timer = FRAMES_BETWEEN_ORBIT_PREDICTION;

  if (timer > 0)
  {
    --timer;
  }
  else if (timer <= 0)
  {
    predict_orbit(
      player_object.position,
      player_velocity,
      player_object.mass,
      sun_object.position,
      sun_object.mass,
      1000
    );

    timer = FRAMES_BETWEEN_ORBIT_PREDICTION;
  }

	return;
}

static void update_sun_position_when_resizing(SDL_Window* p_window)
{
  int screen_width;
  int screen_height;
	
  SDL_GetWindowSize(p_window, &screen_width, &screen_height);

  /* Keeping sun in the center */
  sun_object.position = (Vector2_t) { screen_width / 2, screen_height / 2 };

  return;
}

static bool check_collision_with_sun(void)
{
  float distance_to_sun = vector2_distance(
    player_object.position,
    sun_object.position
  );

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
  /* Open the menu */
  game_state = (e_GameState_t)GAME_STATE_MENU;
	
  /* Reset necessary variables */	
  player_object.position = (Vector2_t) { 80.f, 80.f };
  player_object.velocity = (Vector2_t) { 0.f, 0.f };

  return;
}

static void update(SDL_Window* p_window)
{
  /* Return if we are in the menu */
  /* No physics to update there   */
  if (game_state != (e_GameState_t)GAME_STATE_GAME) return;

  update_sun_position_when_resizing(p_window);
  update_player_direction();
  update_player_orbit_prediction_each_x_frames(apply_gravity());
	
  if (check_collision_with_sun() || is_out_of_screen(p_window, player_object.position))
  {
    player_die();
  }

  return;
}

static void render_planets(SDL_Renderer* p_renderer) // In this case only the sun :P
{
  fill_circle(
    p_renderer,
    (int)sun_object.position.x,
    (int)sun_object.position.y,
    sun_object.radius,
    255, 215, 100,
    255
  );

  return;
}

static void render_player(SDL_Renderer* p_renderer)
{
  /* Making the player a square shape */
  SDL_Rect player = (SDL_Rect) {
    (int)player_object.position.x - player_object.size / 2,
    (int)player_object.position.y - player_object.size / 2,
    player_object.size,
    player_object.size
  };

  SDL_SetRenderDrawColor(p_renderer, 255, 0, 0, 255);
  SDL_RenderFillRect(p_renderer, &player);

  /* Drawing a small line so that we can see where the player is turned */
  SDL_SetRenderDrawColor(p_renderer, 0, 0, 255, 255);
  SDL_RenderDrawLine(
    p_renderer,
    player_object.position.x,
    player_object.position.y,
    player_object.position.x + player_object.forward.x * 15,
    player_object.position.y + player_object.forward.y * 15
  );

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
  /* Setting up a timer for the animation */
  static int timer;
  static bool draw_phrase = false;

  if (timer < 30)
  {
    ++timer;
  }
  else
  {
    draw_phrase = !draw_phrase;
    timer = 0;
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

  if (game_state == (e_GameState_t)GAME_STATE_MENU)
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

