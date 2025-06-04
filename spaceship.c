#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "spaceship.h"

Spaceship CreateSpaceship(const char *filename) {
  Spaceship spaceship = {0};
  spaceship.texture = LoadTexture(filename);
  spaceship.position = (Vector2){(SCREEN_WIDTH - spaceship.texture.width) / 2.0,
                                 SCREEN_HEIGHT / 2.0};
  spaceship.direction = (Vector2){0};
  return spaceship;
}

void UpdateSpaceshipPosition(Spaceship *spaceship, float dt) {
  spaceship->position.x += dt * SPACESHIP_SPEED * spaceship->direction.x;
  spaceship->position.x =
      Clamp(spaceship->position.x, 0, SCREEN_WIDTH - spaceship->texture.width);
  spaceship->position.y += dt * SPACESHIP_SPEED * spaceship->direction.y;
  spaceship->position.y = Clamp(spaceship->position.y, 0,
                                SCREEN_HEIGHT - spaceship->texture.height);
}

void DeleteSpaceship(Spaceship *spaceship) {
  UnloadTexture(spaceship->texture);
}

Star *CreateStars(Texture2D *texture) {
  Star *stars = (Star *)malloc(sizeof(Star) * NUM_STARS);
  for (size_t i = 0; i < NUM_STARS; i++) {
    // TODO: perhaps load the texture only once since its the same asset?
    stars[i].texture = texture;
    stars[i].position = (Vector2){.x = RANDINT(0, SCREEN_WIDTH),
                                  .y = RANDINT(0, SCREEN_HEIGHT)};
    stars[i].scale = RANDFLOAT(0.5, 1.6);
  }
  return stars;
}

void DeleteStars(Star *stars) {
  for (size_t i = 0; i < NUM_STARS; i++) {
    UnloadTexture(*stars[i].texture);
  }
  free(stars);
}

Laser *CreateLaser(Texture2D *texture) {
  Laser *lasers = (Laser *)malloc(sizeof(Laser) * NUM_LASERS);
  for (size_t i = 0; i < NUM_LASERS; i++) {
    // TODO: perhaps load the texture only once since its the same asset?
    lasers[i].texture = texture;
    lasers[i].position = (Vector2){0};
    lasers[i].inview = false;
  }
  return lasers;
}

void UpdateLasers(Laser *lasers, float dt) {
  for (size_t i = 0; i < NUM_LASERS; i++) {
    if (lasers[i].inview)
      lasers[i].position.y -= dt * LASER_SPEED;

    if (lasers[i].position.y < 0)
      lasers[i].inview = false;
  }
}

void DeleteLasers(Laser *lasers) {
  for (size_t i = 0; i < NUM_LASERS; i++) {
    UnloadTexture(*lasers[i].texture);
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

  Texture2D star_texture = LoadTexture("assets/star.png");
  Texture2D laser_texture = LoadTexture("assets/laser.png");

  Star *stars = CreateStars(&star_texture);
  Laser *lasers = CreateLaser(&laser_texture);

  int laser_idx = 0;

  while (!WindowShouldClose()) {
    //  -----------------------------------------------------------------------
    // handling user input
    spaceship.direction.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
    spaceship.direction.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
    spaceship.direction = Vector2Normalize(spaceship.direction);

    if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      laser_idx = (laser_idx + 1) % NUM_LASERS;
      lasers[laser_idx].inview = true;
      lasers[laser_idx].position =
          (Vector2){.x = spaceship.position.x + spaceship.texture.width / 2.2,
                    .y = spaceship.position.y - spaceship.texture.height / 2.0};
    }

    //  -----------------------------------------------------------------------
    // updating the frame
    float dt = GetFrameTime(); // delta time
    UpdateSpaceshipPosition(&spaceship, dt);
    UpdateLasers(lasers, dt);

    //  -----------------------------------------------------------------------
    // handle drawing on the screen
    BeginDrawing();
    {
      ClearBackground(BACKGROUND_COLOR);
      for (size_t i = 0; i < NUM_STARS; i++)
        DrawTextureEx(*stars[i].texture, stars[i].position, 0.0, stars[i].scale,
                      WHITE);
      DrawTextureV(spaceship.texture, spaceship.position, WHITE);
      for (size_t i = 0; i < NUM_LASERS; i++) {
        if (lasers[i].inview)
          DrawTextureV(*lasers[i].texture, lasers[i].position, WHITE);
      }
      DrawFPS(0, 0);
    }
    EndDrawing();
  }

  // ----------------------------------------------------------------------
  // cleanup
  DeleteSpaceship(&spaceship);
  DeleteLasers(lasers);
  DeleteStars(stars);
  CloseWindow();
}
