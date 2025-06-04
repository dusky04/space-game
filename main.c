#include "raylib.h"

int main() {
  InitWindow(800, 600, "Ray");

  // load image into VRAM
  // textures can't be modified again
  Texture2D ship_texture = LoadTexture("assets/spaceship.png");
  Vector2 ship_pos = {.x = 0, .y = 0};
  Vector2 direction = {.x = 1, .y = 1};
  float ship_speed = 200.0;

  while (!WindowShouldClose()) {
    // updating
    float dt = GetFrameTime(); // delta time
    ship_pos.x += ship_speed * dt * direction.x;
    ship_pos.y += ship_speed * dt * direction.y;

    if (0 > ship_pos.x || ship_pos.x > GetScreenWidth() - ship_texture.width)
      direction.x = -direction.x;

    if (0 > ship_pos.y || ship_pos.y > GetScreenHeight() - ship_texture.height)
      direction.y = -direction.y;

    // handles drawing objects to the screen
    BeginDrawing();
    {
      ClearBackground(BLACK);
      DrawTextureV(ship_texture, ship_pos, WHITE);
      DrawFPS(0, 0);
    }
    EndDrawing();
  }
}