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
  spaceship.collision_radius = spaceship.texture.height / 2.0;
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

Vector2 GetSpaceshipCenter(Spaceship *spaceship) {
  return (Vector2){.x = spaceship->position.x + spaceship->texture.width / 2.0,
                   .y =
                       spaceship->position.y + spaceship->texture.height / 2.0};
}

Star *CreateStars() {
  Star *stars = (Star *)malloc(sizeof(Star) * NUM_STARS);
  for (size_t i = 0; i < NUM_STARS; i++) {
    stars[i].position = (Vector2){.x = RANDINT(0, SCREEN_WIDTH),
                                  .y = RANDINT(0, SCREEN_HEIGHT)};
    stars[i].scale = RANDFLOAT(0.5, 1.6);
  }
  return stars;
}

void DeleteStars(Star *stars) { free(stars); }

Laser *CreateLasers() {
  Laser *lasers = (Laser *)malloc(sizeof(Laser) * NUM_LASERS);
  for (size_t i = 0; i < NUM_LASERS; i++) {
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
    asteroids[i].position = (Vector2){0};
    asteroids[i].direction = (Vector2){0};
    asteroids[i].inview = false;
    asteroids[i].rotation = 0.0;
    asteroids[i].collision_radius = texture->height / 2.0;
  }
  return asteroids;
}

void UpdateAsteroid(Asteroid *asteroids, float dt) {
  for (size_t i = 0; i < NUM_ASTEROIDS; i++) {
    if (asteroids[i].inview) {
      asteroids[i].position.x += dt * ASTEROID_SPEED * asteroids[i].direction.x;
      asteroids[i].position.y += dt * ASTEROID_SPEED * asteroids[i].direction.y;
      asteroids[i].rotation += dt * ASTEROID_ROTATION_SPEED;
    }
    if (asteroids[i].inview &&
        (asteroids[i].position.x < 0 || asteroids[i].position.y < 0)) {
      asteroids[i].inview = false;
    }
  }
}

Vector2 GetAsteroidCenter(Asteroid *asteroid, Texture2D *texture) {
  return (Vector2){.x = asteroid->position.x + texture->width / 2.0,
                   .y = asteroid->position.y + texture->height / 2.0};
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

int SpawnAsteroid(Asteroid *asteroids, int asteroid_idx) {
  asteroids[asteroid_idx].inview = true;
  asteroids[asteroid_idx].position =
      (Vector2){.x = RANDINT(0, SCREEN_WIDTH), .y = 0};
  asteroids[asteroid_idx].direction =
      (Vector2){.x = RANDFLOAT(-0.5, 0.5), .y = 1};
  return (asteroid_idx + 1) % NUM_ASTEROIDS;
}

int UpdateTimer(Timer *timer, Asteroid *asteroids, int asteroid_idx, double t) {
  if (timer->active) {
    if (t - timer->start_time >= timer->duration) {
      asteroid_idx = SpawnAsteroid(asteroids, asteroid_idx);
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

  Star *stars = CreateStars();
  Laser *lasers = CreateLasers();
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
    if (spaceship.position.x != 0 || spaceship.position.y != 0)
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

    double t = GetTime();
    asteroid_idx = UpdateTimer(&timer, asteroids, asteroid_idx, t);
    UpdateAsteroid(asteroids, dt);

    // checking collision between spaceship and meteor
    for (size_t i = 0; i < NUM_ASTEROIDS; i++) {
      if (asteroids[i].inview) {
        if (CheckCollisionCircles(
                GetSpaceshipCenter(&spaceship), spaceship.collision_radius,
                GetAsteroidCenter(&asteroids[i], &asteroid_texture),
                asteroids[i].collision_radius)) {
        }
      }
    }
    //  -----------------------------------------------------------------------
    // handle drawing on the screen
    BeginDrawing();
    {
      ClearBackground(BACKGROUND_COLOR);
      for (size_t i = 0; i < NUM_STARS; i++)
        DrawTextureEx(star_texture, stars[i].position, 0.0, stars[i].scale,
                      WHITE);
      DrawTextureV(spaceship.texture, spaceship.position, WHITE);
      for (size_t i = 0; i < NUM_LASERS; i++) {
        if (lasers[i].inview)
          DrawTextureV(laser_texture, lasers[i].position, WHITE);
      }
      for (size_t i = 0; i < NUM_ASTEROIDS; i++) {
        if (asteroids[i].inview)
          DrawTextureEx(asteroid_texture, asteroids[i].position,
                        asteroids[i].rotation, 1, WHITE);
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
