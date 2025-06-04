#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <time.h>

#define NUM_CIRCLES 100

#define RANDINT(min, max) (rand() % (max - min + 1) + min)

typedef struct {
  Vector2 center;
  float radius;
  Color color;
} Circle;

int main() {
  srand(time(NULL));

  InitWindow(1000, 600, "Camera");

  // player
  Vector2 player_pos = {0};
  int radius = 50;
  Vector2 direction = {0};
  int speed = 400;

  // create circles
  Circle *circles = (Circle *)malloc(sizeof(Circle) * NUM_CIRCLES);
  for (size_t i = 0; i < NUM_CIRCLES; i++) {
    circles[i].center =
        (Vector2){.x = RANDINT(-2000, 600), .y = RANDINT(-1000, 1000)};
    circles[i].radius = RANDINT(50, 200);
    circles[i].color = (Color){.r = RANDINT(0, 255),
                               .g = RANDINT(0, 255),
                               .b = RANDINT(0, 255),
                               .a = 255};
  }

  // create camera
  Camera2D camera = {0};
  camera.zoom = 1.0;
  camera.target = player_pos;
  camera.offset =
      (Vector2){.x = GetScreenWidth() / 2.0, .y = GetScreenHeight() / 2.0};

  while (!WindowShouldClose()) {
    // detect player input
    direction.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
    direction.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
    direction = Vector2Normalize(direction);

    // movement of the player
    float dt = GetFrameTime();
    player_pos.x += dt * speed * direction.x;
    player_pos.y += dt * speed * direction.y;

    // update camera
    camera.target = player_pos;
    camera.zoom += dt * 10 * (IsKeyDown(KEY_W) - IsKeyDown(KEY_Q));

    BeginDrawing();
    BeginMode2D(camera);
    {
      ClearBackground(WHITE);
      // draw the circles
      for (size_t i = 0; i < NUM_CIRCLES; i++)
        DrawCircleV(circles[i].center, circles[i].radius, circles[i].color);
      DrawCircleV(player_pos, radius, BLACK);
    }
    EndMode2D();
    EndDrawing();
  }
  CloseWindow();
}