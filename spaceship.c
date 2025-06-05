#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "raylib.h"
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

Star *CreateStars(Texture2D *texture) {
  Star *stars = (Star *)malloc(sizeof(Star) * NUM_STARS);
  for (size_t i = 0; i < NUM_STARS; i++) {
    stars[i].texture = texture;
    stars[i].position = (Vector2){.x = RANDINT(0, SCREEN_WIDTH),
                                  .y = RANDINT(0, SCREEN_HEIGHT)};
    stars[i].scale = RANDFLOAT(0.5, 1.6);
  }
  return stars;
}

void DeleteStars(Star *stars) { free(stars); }

Laser *CreateLasers(Texture2D *texture) {
  Laser *lasers = (Laser *)malloc(sizeof(Laser) * NUM_LASERS);
  for (size_t i = 0; i < NUM_LASERS; i++) {
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

void DeleteLasers(Laser *lasers) { free(lasers); }

Asteroid *CreateAsteroids(Texture2D *texture) {
  Asteroid *asteroids = (Asteroid *)malloc(sizeof(Asteroid) * NUM_ASTEROIDS);
  for (size_t i = 0; i < NUM_ASTEROIDS; i++) {
    asteroids[i].texture = texture;
    asteroids[i].position = (Vector2){0};
    asteroids[i].direction = (Vector2){0};
    asteroids[i].inview = false;
  }
  return asteroids;
}

void UpdateAsteroid(Asteroid *asteroids, float dt) {
  for (size_t i = 0; i < NUM_ASTEROIDS; i++) {
    if (asteroids[i].inview) {
      asteroids[i].position.x += dt * ASTEROID_SPEED * asteroids[i].direction.x;
      asteroids[i].position.y += dt * ASTEROID_SPEED * asteroids[i].direction.y;
    }
    if (asteroids[i].inview &&
        (asteroids[i].position.x < 0 || asteroids[i].position.y < 0)) {
      asteroids[i].inview = false;
    }
  }
}

Timer CreateTimer(double duration, bool repeat, bool autostart) {
  Timer timer = {0};
  timer.duration = duration;
  timer.start_time = -1.0; // timer hasn't started yet
  timer.active = false;
  timer.repeat = repeat;
  if (autostart) {
    timer.active = true;
    StartTimer(&timer);
  }
  return timer;
}

void StartTimer(Timer *timer) {
  timer->active = true;
  timer->start_time = GetTime();
}

void StopTimer(Timer *timer) {
  timer->active = false;
  timer->start_time = -1.0;
  if (timer->repeat)
    StartTimer(timer);
}

int DrawAsteroid(Asteroid *asteroids, int asteroid_idx) {
  asteroids[asteroid_idx].inview = true;
  asteroids[asteroid_idx].position =
      (Vector2){.x = RANDINT(0, SCREEN_WIDTH), 0};
  asteroids[asteroid_idx].direction =
      (Vector2){.x = RANDFLOAT(-0.5, 0.5), .y = 1};
  return (asteroid_idx + 1) % NUM_ASTEROIDS;
}

int UpdateTimer(Timer *timer, Asteroid *asteroids, int asteroid_idx) {
  if (timer->active) {
    if (GetTime() - timer->start_time >= timer->duration) {
      asteroid_idx = DrawAsteroid(asteroids, asteroid_idx);
      StopTimer(timer);
    }
  }
  return asteroid_idx;
}

int main() {
  srand(time(NULL));

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space");
  InitAudioDevice();
  SetExitKey(KEY_ESCAPE);
  SetTargetFPS(60);

  // load the textures
  Spaceship spaceship = CreateSpaceship("assets/spaceship.png");
  Texture2D star_texture = LoadTexture("assets/star.png");
  Texture2D laser_texture = LoadTexture("assets/laser.png");
  Texture2D asteroid_texture = LoadTexture("assets/asteroid.png");

  Star *stars = CreateStars(&star_texture);
  Laser *lasers = CreateLasers(&laser_texture);
  Asteroid *asteroids = CreateAsteroids(&asteroid_texture);

  // load audio
  Sound laser_sound = LoadSound("assets/laser.wav");

  // start the timer
  Timer timer = CreateTimer(ASTEROID_DURATION, true, true);

  int laser_idx = 0;
  int asteroid_idx = 0;
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
      PlaySound(laser_sound);
    }

    //  -----------------------------------------------------------------------
    // updating the frame
    float dt = GetFrameTime(); // delta time
    UpdateSpaceshipPosition(&spaceship, dt);
    UpdateLasers(lasers, dt);
    asteroid_idx = UpdateTimer(&timer, asteroids, asteroid_idx);
    UpdateAsteroid(asteroids, dt);

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
      for (size_t i = 0; i < NUM_ASTEROIDS; i++) {
        if (asteroids[i].inview)
          DrawTextureV(*asteroids[i].texture, asteroids[i].position, WHITE);
      }

      DrawFPS(0, 0);
    }
    EndDrawing();
  }

  // ----------------------------------------------------------------------
  // cleanup
  UnloadSound(laser_sound);
  UnloadTexture(spaceship.texture);
  UnloadTexture(star_texture);
  UnloadTexture(laser_texture);
  UnloadTexture(asteroid_texture);
  DeleteLasers(lasers);
  DeleteStars(stars);
  CloseAudioDevice();
  CloseWindow();
}
