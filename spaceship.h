#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 750

#define SPACESHIP_SPEED 400
#define LASER_SPEED 600
#define ASTEROID_SPEED 400
#define ASTEROID_ROTATION_SPEED 50

#define NUM_STARS 30
#define NUM_LASERS 10
#define NUM_ASTEROIDS 10
#define NUM_EXPLOSIONS 10

#define ASTEROID_DURATION 0.4
#define BOOM_ANIMATION_SPEED 10

#define BOOM_ANIMATION_FRAMES 28

#define BACKGROUND_COLOR                                                       \
  CLITERAL(Color) { 28, 22, 37, 255 }

#define RANDINT(min, max) (rand() % (max - min + 1) + min)
#define RANDFLOAT(min, max) (((max - min) * (float)rand() / RAND_MAX) + min)

#define MOD(idx, max) ((idx + 1) % max)

typedef struct {
  Texture2D *texture;
  Vector2 position;
  Vector2 direction;
  float collision_radius;
} Spaceship;

typedef struct {
  Vector2 position;
  float scale;
} Star;

typedef struct {
  Vector2 position;
  Rectangle rec;
  bool inview;
} Laser;

typedef struct {
  Vector2 position;
  Vector2 direction;
  Rectangle rec;
  bool inview;
  double rotation;
  float collision_radius;
} Asteroid;

typedef struct {
  Vector2 size;
  Vector2 position;
  bool inview;
  int index;
} Boom;

typedef struct {
  Texture2D ship_texture;
  Texture2D star_texture;
  Texture2D laser_texture;
  Texture2D asteroid_texture;
  Texture2D boom_textures[BOOM_ANIMATION_FRAMES];
  Sound laser_sound;
} Assets;

typedef struct {
  Spaceship ship;
  Star stars[NUM_STARS];
  Laser lasers[NUM_LASERS];
  Boom booms[NUM_EXPLOSIONS];
  Asteroid asteroids[NUM_ASTEROIDS];
  int boom_idx;
  int laser_idx;
  int asteroid_idx;
} Game;

void CreateSpaceship(Game *game, Assets *assets);
void UpdateSpaceship(Game *game, float dt);
Vector2 GetSpaceShipCenter(Game *game);

void CreateStars(Game *game);

void CreateLasers(Game *game);
void UpdateLasers(Laser *lasers, float dt);

void CreateAsteroids(Game *game, Assets *textures);
void UpdateAsteroid(Asteroid *asteroids, float dt);
Vector2 GetAsteroidCenter(Asteroid *asteroid);
int SpawnAsteroid(Asteroid *asteroids, int asteroid_idx);

typedef struct {
  double duration;
  double start_time;
  bool active;
  bool repeat;
} Timer;

Timer CreateTimer(double duration, bool repeat, bool autostart);
void StartTimer(Timer *timer);
void StopTimer(Timer *timer);
int UpdateTimer(Timer *timer, Asteroid *asteroids, int asteroid_idx, double t);

void CreateBooms(Game *game, Assets *textures);
int DrawBoom(Boom *booms, int boom_idx, Vector2 position);
void UpdateBoom(Boom *booms, float dt);

#endif