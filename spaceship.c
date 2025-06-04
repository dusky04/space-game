#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 750

#define SPACESHIP_SPEED 400

#define LASER_LENGTH 50
#define LASER_SPEED 600

#define NUM_STARS 30
#define NUM_LASERS 10

#define BACKGROUND_COLOR                                                       \
  CLITERAL(Color) { 28, 22, 37, 255 }

#define RANDINT(min, max) (rand() % (max - min + 1) + min)
#define RANDFLOAT(min, max) (((max - min) * (float)rand() / RAND_MAX) + min)

typedef struct {
  Texture2D texture;
  Vector2 position;
  Vector2 direction;
} Spaceship;

Spaceship CreateSpaceship(const char *filename) {
  Spaceship spaceship = {0};
  spaceship.texture = LoadTexture(filename);
  spaceship.position = (Vector2){(SCREEN_WIDTH - spaceship.texture.width) / 2.0,
                                 SCREEN_HEIGHT / 2.0};
  spaceship.direction = (Vector2){0};
  return spaceship;
}

typedef struct {
  Texture2D texture;
  Vector2 position;
  float scale;
} Star;

Star *CreateStars(const char *filename) {
  Star *stars = (Star *)malloc(sizeof(Star) * NUM_STARS);
  for (size_t i = 0; i < NUM_STARS; i++) {
    // TODO: perhaps load the texture only once since its the same asset?
    stars[i].texture = LoadTexture(filename);
    stars[i].position = (Vector2){.x = RANDINT(0, SCREEN_WIDTH),
                                  .y = RANDINT(0, SCREEN_HEIGHT)};
    stars[i].scale = RANDFLOAT(0.5, 1.6);
  }
  return stars;
}

void DeleteStars(Star *stars) {
  for (size_t i = 0; i < NUM_STARS; i++) {
    UnloadTexture(stars[i].texture);
  }
  free(stars);
}

typedef struct {
  Texture2D texture;
  Vector2 position;
  bool inview;
} Laser;

Laser *CreateLaser(const char *filename) {
  Laser *lasers = (Laser *)malloc(sizeof(Laser) * NUM_LASERS);
  for (size_t i = 0; i < NUM_STARS; i++) {
    // TODO: perhaps load the texture only once since its the same asset?
    lasers[i].texture = LoadTexture(filename);
    lasers[i].position = (Vector2){0};
    lasers[i].inview = false;
  }
  return lasers;
}

void DeleteLasers(Laser *lasers) {
  for (size_t i = 0; i < NUM_LASERS; i++) {
    UnloadTexture(lasers[i].texture);
  }
  free(lasers);
}

int main() {
  srand(time(NULL));

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space");
  SetExitKey(KEY_ESCAPE);
  SetTargetFPS(60);

  // load the spaceship texture
  Spaceship spaceship = CreateSpaceship("assets/spaceship.png");
  Star *stars = CreateStars("assets/star.png");
  Laser *lasers = CreateLaser("assets/laser.png");

  int laser_idx = 0;

  while (!WindowShouldClose()) {
    //  -----------------------------------------------------------------------
    // updating the frame
    spaceship.direction.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
    spaceship.direction.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
    spaceship.direction = Vector2Normalize(spaceship.direction);

    if (IsKeyPressed(KEY_SPACE)) {
      laser_idx = (laser_idx + 1) % NUM_LASERS;
      lasers[laser_idx].inview = true;
      lasers[laser_idx].position =
          (Vector2){.x = spaceship.position.x, .y = spaceship.position.y};
    }

    // TODO: refactor it into a method
    // update player position
    float dt = GetFrameTime(); // delta time
    spaceship.position.x += dt * SPACESHIP_SPEED * spaceship.direction.x;
    spaceship.position.x =
        Clamp(spaceship.position.x, 0, SCREEN_WIDTH - spaceship.texture.width);
    spaceship.position.y += dt * SPACESHIP_SPEED * spaceship.direction.y;
    spaceship.position.y = Clamp(spaceship.position.y, 0,
                                 SCREEN_HEIGHT - spaceship.texture.height);

    // update laser
    for (size_t i = 0; i < NUM_LASERS; i++) {
      if (lasers[i].inview)
        lasers[i].position.y -= dt * LASER_SPEED;

      if (lasers[i].position.y < 0)
        lasers[i].inview = false;
    }

    //  -----------------------------------------------------------------------
    // handle drawing on the screen
    BeginDrawing();
    {
      ClearBackground(BACKGROUND_COLOR);
      for (size_t i = 0; i < NUM_STARS; i++)
        DrawTextureEx(stars[i].texture, stars[i].position, 0.0, stars[i].scale,
                      WHITE);
      DrawTextureV(spaceship.texture, spaceship.position, WHITE);
      for (size_t i = 0; i < NUM_LASERS; i++) {
        if (lasers[i].inview)
          DrawTextureV(lasers[i].texture, lasers[i].position, WHITE);
      }
      DrawFPS(0, 0);
    }
    EndDrawing();
  }

  // ----------------------------------------------------------------------
  // cleanup
  UnloadTexture(spaceship.texture);
  DeleteLasers(lasers);
  DeleteStars(stars);
  CloseWindow();
}

// when i press the left mouse button
// i should fire a laser