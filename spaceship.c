#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "raylib.h"
#include "spaceship.h"

// TODO: improve asteroid collision with laser
// TODO: improve the behavior of explosions

void CreateSpaceship(Game *game, Assets *assets) {
  game->ship.texture = &assets->ship_texture;
  game->ship.position = (Vector2){
      (SCREEN_WIDTH - game->ship.texture->width) / 2.0, SCREEN_HEIGHT / 2.0};
  game->ship.direction = (Vector2){0};
  game->ship.collision_radius = game->ship.texture->height / 2.0;
}

void UpdateSpaceship(Game *game, float dt) {
  game->ship.position.x += dt * SPACESHIP_SPEED * game->ship.direction.x;
  game->ship.position.x =
      Clamp(game->ship.position.x, 0, SCREEN_WIDTH - game->ship.texture->width);
  game->ship.position.y += dt * SPACESHIP_SPEED * game->ship.direction.y;
  game->ship.position.y = Clamp(game->ship.position.y, 0,
                                SCREEN_HEIGHT - game->ship.texture->height);
}

Vector2 GetSpaceshipCenter(Game *game) {
  return (Vector2){.x = game->ship.position.x + game->ship.texture->width / 2.0,
                   .y = game->ship.position.y +
                        game->ship.texture->height / 2.0};
}

void CreateStars(Game *game) {
  for (size_t i = 0; i < NUM_STARS; i++) {
    game->stars[i].position = (Vector2){.x = RANDINT(0, SCREEN_WIDTH),
                                        .y = RANDINT(0, SCREEN_HEIGHT)};
    game->stars[i].scale = RANDFLOAT(0.5, 1.6);
  }
}

void CreateLasers(Game *game) {
  for (size_t i = 0; i < NUM_LASERS; i++) {
    game->lasers[i].position = (Vector2){0};
    game->lasers[i].inview = false;
    game->lasers[i].rec = (Rectangle){0};
  }
}

void UpdateLasers(Laser *lasers, float dt) {
  for (size_t i = 0; i < NUM_LASERS; i++) {
    if (lasers[i].inview)
      lasers[i].position.y -= dt * LASER_SPEED;

    // if the laser is out of view, don't render it
    if (lasers[i].position.y < 0)
      lasers[i].inview = false;
  }
}

void CreateAsteroids(Game *game, Assets *assets) {
  for (size_t i = 0; i < NUM_ASTEROIDS; i++) {
    game->asteroids[i].position = (Vector2){0};
    game->asteroids[i].direction = (Vector2){0};
    game->asteroids[i].inview = false;
    game->asteroids[i].rotation = 0.0;
    game->asteroids[i].collision_radius = assets->asteroid_texture.height / 2.0;
    game->asteroids[i].rec =
        (Rectangle){.x = 0,
                    .y = 0,
                    .width = assets->asteroid_texture.width,
                    .height = assets->asteroid_texture.height};
  }
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

Vector2 GetAsteroidCenter(Asteroid *asteroid) {
  return (Vector2){.x = asteroid->position.x + asteroid->rec.width / 2.0,
                   .y = asteroid->position.y + asteroid->rec.height / 2.0};
}

int SpawnAsteroid(Asteroid *asteroids, int asteroid_idx) {
  asteroids[asteroid_idx].inview = true;
  asteroids[asteroid_idx].position =
      (Vector2){.x = RANDINT(0, SCREEN_WIDTH), .y = 0};
  asteroids[asteroid_idx].direction =
      (Vector2){.x = RANDFLOAT(-0.5, 0.5), .y = 1};
  return MOD(asteroid_idx, NUM_ASTEROIDS);
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

int UpdateTimer(Timer *timer, Asteroid *asteroids, int asteroid_idx, double t) {
  if (timer->active) {
    if (t - timer->start_time >= timer->duration) {
      asteroid_idx = SpawnAsteroid(asteroids, asteroid_idx);
      StopTimer(timer);
    }
  }
  return asteroid_idx;
}

void CreateBooms(Game *game, Assets *assets) {
  for (size_t i = 0; i < NUM_EXPLOSIONS; i++) {
    game->booms[i].size = (Vector2){.x = assets->boom_textures[0].width,
                                    .y = assets->boom_textures[0].height};
    game->booms[i].position = (Vector2){0};
    game->booms[i].inview = false;
    game->booms[i].index = 0;
  }
}

int DrawBoom(Boom *booms, int boom_idx, Vector2 position) {
  booms[boom_idx].inview = true;
  booms[boom_idx].position = position;
  return MOD(boom_idx, NUM_EXPLOSIONS);
}

void UpdateBoom(Boom *booms, float dt) {
  for (size_t i = 0; i < NUM_EXPLOSIONS; i++) {
    if (booms[i].inview) {
      booms[i].index += (int)(BOOM_ANIMATION_SPEED * dt);
      if (booms[i].index >= BOOM_ANIMATION_FRAMES) {
        booms[i].inview = false;
        booms[i].index = 0;
      }
    }
  }
}

void LoadAssets(Assets *assets) {
  assets->ship_texture = LoadTexture("assets/spaceship.png");
  assets->star_texture = LoadTexture("assets/star.png");
  assets->laser_texture = LoadTexture("assets/laser.png");
  assets->asteroid_texture = LoadTexture("assets/asteroid.png");
  for (size_t i = 0; i < BOOM_ANIMATION_FRAMES; i++) {
    char buffer[30];
    sprintf(buffer, "assets/explosion/%zu.png", i + 1);
    assets->boom_textures[i] = LoadTexture(buffer);
  }
  assets->laser_sound = LoadSound("assets/laser.wav");
}

void InitGame(Game *game, Assets *assets) {
  CreateSpaceship(game, assets);
  CreateStars(game);
  CreateLasers(game);
  CreateAsteroids(game, assets);
  CreateBooms(game, assets);
  game->boom_idx = 0;
  game->laser_idx = 0;
  game->asteroid_idx = 0;
}

void HandleInput(Game *game, Assets *assets) {
  game->ship.direction.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
  game->ship.direction.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
  if (game->ship.position.x != 0 || game->ship.position.y != 0)
    game->ship.direction = Vector2Normalize(game->ship.direction);

  if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    game->laser_idx = MOD(game->laser_idx, NUM_LASERS);
    game->lasers[game->laser_idx].inview = true;
    game->lasers[game->laser_idx].position = (Vector2){
        .x = game->ship.position.x + game->ship.texture->width / 2.2,
        .y = game->ship.position.y - game->ship.texture->height / 2.0};
    game->lasers[game->laser_idx].rec =
        (Rectangle){.x = game->lasers[game->laser_idx].position.x,
                    .y = game->lasers[game->laser_idx].position.y,
                    .width = assets->laser_texture.width,
                    .height = assets->laser_texture.height};
    PlaySound(assets->laser_sound);
  }
}

void UpdateGame(Game *game, Timer *timer, Assets *assets, float dt) {
  UpdateSpaceship(game, dt);
  UpdateLasers(game->lasers, dt);
  UpdateBoom(game->booms, dt);

  double t = GetTime();
  game->asteroid_idx =
      UpdateTimer(timer, game->asteroids, game->asteroid_idx, t);
  UpdateAsteroid(game->asteroids, dt);

  // checking collision between spaceship and meteor
  for (size_t i = 0; i < NUM_ASTEROIDS; i++) {
    if (game->asteroids[i].inview) {
      if (CheckCollisionCircles(GetSpaceshipCenter(game),
                                game->ship.collision_radius,
                                GetAsteroidCenter(&game->asteroids[i]),
                                game->asteroids[i].collision_radius)) {
        printf("Collsion");
      }
    }
  }

  // checking collison between laser and meteor
  for (size_t i = 0; i < NUM_LASERS; i++) {
    for (size_t j = 0; j < NUM_ASTEROIDS; j++) {
      if (game->lasers[i].inview && game->asteroids[j].inview) {
        if (CheckCollisionCircleRec(GetAsteroidCenter(&game->asteroids[j]),
                                    game->asteroids[j].collision_radius,
                                    game->lasers[i].rec)) {
          game->lasers[i].inview = false;
          game->asteroids[j].inview = false;
          game->boom_idx = DrawBoom(game->booms, game->boom_idx,
                                    (Vector2){.x = game->lasers[i].position.x -
                                                   assets->laser_texture.width,
                                              .y = game->lasers[i].position.y});
        }
      }
    }
  }
}

void DrawGame(Game *game, Assets *assets) {
  BeginDrawing();
  {
    ClearBackground(BACKGROUND_COLOR);
    for (size_t i = 0; i < NUM_STARS; i++)
      DrawTextureEx(assets->star_texture, game->stars[i].position, 0.0,
                    game->stars[i].scale, WHITE);
    DrawTextureV(assets->ship_texture, game->ship.position, WHITE);
    for (size_t i = 0; i < NUM_LASERS; i++) {
      if (game->lasers[i].inview)
        DrawTextureV(assets->laser_texture, game->lasers[i].position, WHITE);
    }

    for (size_t i = 0; i < NUM_ASTEROIDS; i++) {
      if (game->asteroids[i].inview)
        DrawTexturePro(assets->asteroid_texture, game->asteroids[i].rec,
                       (Rectangle){.x = game->asteroids[i].position.x,
                                   .y = game->asteroids[i].position.y,
                                   .width = assets->asteroid_texture.width,
                                   .height = assets->asteroid_texture.height},
                       (Vector2){assets->laser_texture.width / 2.0,
                                 assets->laser_texture.height / 2.0},
                       game->asteroids[i].rotation, WHITE);
    }
    for (size_t i = 0; i < NUM_EXPLOSIONS; i++) {
      if (game->booms[i].inview) {
        DrawTextureEx(assets->boom_textures[game->booms[i].index],
                      game->booms[i].position, 0.0, 1.0, WHITE);
      }
    }
    DrawFPS(0, 0);
  }
  EndDrawing();
}

void DeinitGame(Assets *assets) {
  UnloadTexture(assets->ship_texture);
  UnloadTexture(assets->star_texture);
  UnloadTexture(assets->laser_texture);
  UnloadTexture(assets->asteroid_texture);
  for (size_t i = 0; i < BOOM_ANIMATION_FRAMES; i++) {
    UnloadTexture(assets->boom_textures[i]);
  }
  UnloadSound(assets->laser_sound);
}

int main() {
  srand(time(NULL));

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space");
  InitAudioDevice();
  SetExitKey(KEY_ESCAPE);
  SetTargetFPS(60);

  // load the assets
  Assets assets = {0};
  LoadAssets(&assets);

  Game game = {0};
  InitGame(&game, &assets);

  // start the timer
  Timer timer = CreateTimer(ASTEROID_DURATION, true, true);

  while (!WindowShouldClose()) {
    HandleInput(&game, &assets);
    UpdateGame(&game, &timer, &assets, GetFrameTime());
    DrawGame(&game, &assets);
  }

  DeinitGame(&assets);
  CloseAudioDevice();
  CloseWindow();
}
