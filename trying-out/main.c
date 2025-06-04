#include "raylib.h"
#include "raymath.h"

int main() {
  InitWindow(800, 600, "Ray");
  SetExitKey(KEY_ESCAPE);

  // load image into VRAM
  // textures can't be modified again
  Texture2D ship_texture = LoadTexture("assets/spaceship.png");
  Vector2 ship_pos = {.x = 0, .y = 0};
  Vector2 direction = {.x = 1, .y = 1};
  float ship_speed = 200.0;

  while (!WindowShouldClose()) {

    // move ship using keys
    direction.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
    direction.y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
    // normalize direction vector to length 1 for consistent movement speed
    // otherwise it would travel by sqrt(2)
    direction = Vector2Normalize(direction);

    float dt = GetFrameTime(); // delta time
    ship_pos.x += ship_speed * dt * direction.x;
    ship_pos.y += ship_speed * dt * direction.y;

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