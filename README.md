# Space Shooter Game

A classic arcade-style space shooter game built in C using the Raylib graphics library. Control your spaceship, dodge asteroids, and blast them out of the sky with lasers!

## Game Controls

| Key/Input | Action |
|-----------|--------|
| `W` | Move spaceship up |
| `A` | Move spaceship left |
| `S` | Move spaceship down |
| `D` | Move spaceship right |
| `Space` | Fire laser |
| `Left Mouse` | Fire laser |
| `Escape` | Exit game |


**Ubuntu/Debian:**
```bash
gcc -Iraylib-5.5_linux_amd64/include -o spaceship spaceship.c -Lraylib-5.5_linux_amd64/lib -l:libraylib.a -lm 
```
```bash
./spaceship
```

## TODOs

### Bugs
- [ ] Improve asteroid collision with laser
- [ ] Figure out why explosions aren't appearing
- [ ] Fix boom index calculation (should use `%` not `&` in DrawBoom function)
- [ ] Fix spaceship movement normalization condition (should check if direction is non-zero, not position)

### Missing Features
- [ ] Add game over screen when spaceship collides with asteroid
- [ ] Add scoring system
- [ ] Add sound effect for explosions
- [ ] Add sound effect for asteroid destruction
- [ ] Add background music

### Code Improvements
- [ ] Add proper memory cleanup for boom textures
- [ ] Add bounds checking for asteroid movement (currently only checks if < 0)
- [ ] Add pause functionality
- [ ] Implement proper game states (menu, playing, game over)