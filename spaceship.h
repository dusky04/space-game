#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 750

#define SPACESHIP_SPEED 400
#define LASER_SPEED 600
#define ASTEROID_SPEED 400

#define NUM_STARS 30
#define NUM_LASERS 10
#define NUM_ASTEROIDS 10

#define ASTEROID_DURATION 0.4

#define BACKGROUND_COLOR                                                       \
  CLITERAL(Color) { 28, 22, 37, 255 }

#define RANDINT(min, max) (rand() % (max - min + 1) + min)
#define RANDFLOAT(min, max) (((max - min) * (float)rand() / RAND_MAX) + min)

typedef struct {
  Texture2D texture;
  Vector2 position;
  Vector2 direction;
} Spaceship;

Spaceship CreateSpaceship(const char *filename);
void UpdateSpaceshipPosition(Spaceship *spaceship, float dt);

typedef struct {
  Texture2D *texture;
  Vector2 position;
  float scale;
} Star;

Star *CreateStars(Texture2D *texture);
void DeleteStars(Star *stars);

typedef struct {
  Texture2D *texture;
  Vector2 position;
  bool inview;
} Laser;

Laser *CreateLasers(Texture2D *texture);
void UpdateLasers(Laser *lasers, float dt);
void DeleteLasers(Laser *lasers);

typedef struct {
  Texture2D *texture;
  Vector2 position;
  Vector2 direction;
  bool inview;
} Asteroid;

typedef struct {
  double duration;
  double start_time;
  bool active;
  bool repeat;
} Timer;

Timer CreateTimer(double duration, bool repeat, bool autostart);
void StartTimer(Timer *timer);

#endif