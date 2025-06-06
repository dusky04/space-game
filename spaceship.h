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

#define BACKGROUND_COLOR                                                       \
  CLITERAL(Color) { 28, 22, 37, 255 }

#define RANDINT(min, max) (rand() % (max - min + 1) + min)
#define RANDFLOAT(min, max) (((max - min) * (float)rand() / RAND_MAX) + min)

typedef struct {
  Texture2D texture;
  Vector2 position;
  Vector2 direction;
  float collision_radius;
} Spaceship;

Spaceship CreateSpaceship(const char *filename);
void UpdateSpaceship(Spaceship *spaceship, float dt);
Vector2 GetSpaceShipCenter(Spaceship *spaceship);

typedef struct {
  Vector2 position;
  float scale;
} Star;

Star *CreateStars();

typedef struct {
  Vector2 position;
  Rectangle rec;
  bool inview;
} Laser;

Laser *CreateLasers();
void UpdateLasers(Laser *lasers, float dt);

typedef struct {
  Vector2 position;
  Vector2 direction;
  Rectangle rec;
  bool inview;
  double rotation;
  float collision_radius;
} Asteroid;

Asteroid *CreateAsteroids(Texture2D *texture);
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

typedef struct {
  Vector2 size;
  Vector2 position;
  bool inview;
  int index;
} Boom;

Boom *CreateBooms(Texture2D *texture);
int DrawBoom(Boom *booms, int boom_idx, Vector2 position);
void UpdateBoom(Boom *booms, float dt);

#endif