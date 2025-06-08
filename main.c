#include "spaceship.h"

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

  UnloadAssets(&assets);
  CloseAudioDevice();
  CloseWindow();
}