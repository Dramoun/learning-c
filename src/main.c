#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include "math/body.h"
#include "utils/terminal_utils.h"
#include "utils/general.h"
#include "components/structures.h"
#include "components/units.h"
#include "types.h"

#define _POSIX_C_SOURCE 199309L


void renderMap(Game *game);
void gamePlaying(Game *game, KeyState keys[KEY_COUNT], float *bulletLimiter);
void placeUnit(size_t maxX, char buffer[][maxX], Game *game, Unit unit);

int main() {
  Game *game = createGame();
  f32 bulletLimiter = 0.0f;
  
  InitKeyPair keyPair = getInitKeys();

  if (!game) {
    printf("Failed to initialize game\n");
    return 1;
  }

  enableSpecialTerminal();
    

  f64 currentTime = timeNow();
  f64 accumulator = 0.0;
  const f64 dt = game->secondsPerFrame;
 
  while (game->isRunning && !shouldQuit()) {
    f64 frameStart = timeNow();
    f64 frameTime = frameStart - currentTime;
    currentTime = frameStart;
    
    if (frameTime > 0.25) {
      frameTime = 0.25;
    }

    beginInputFrame(keyPair.keys);
    updateInput(keyPair.keys, keyPair.lastSeen);

    accumulator += frameTime;
    i8 updateCount = 0;
    const i8 maxUpdates = 5;
   
    while (accumulator >= dt && updateCount < maxUpdates) {
      // Updates here
      gamePlaying(game, keyPair.keys, &bulletLimiter);
      bodyUpdate(&game->player->body, dt);
      game->player->body.vel = (Vec2) {0,0};
      accumulator -= dt;
      updateCount++;
     }

    if (updateCount == maxUpdates ) {
      accumulator = 0;
    }
     
    //RENDER HERE
    printf("\033[H\033[J"); // clear screen
    renderMap(game);
    
    double elapsed = timeNow() - frameStart;
     
    if (elapsed < dt) {
      sleepMs((dt - elapsed) * 1000);
    }
  }
  
  destroyGame(game);
  disableSpecialTerminal();

  return 0;
};

// TODO: need to separate update and render logics, we should be updateing more then once per frame
void gamePlaying(Game *game, KeyState keys[KEY_COUNT], float *bulletLimiter){
  if (keys[KEY_Q].pressed) {
    game->isRunning = 0;
    return;
  }
 
  // USER POS UPDATES
  if (keys[KEY_A].down) { // left
    playerMoveLeft(game->player);
  }
  if (keys[KEY_D].down) { // right
    playerMoveRight(game->player);
  }
  if (keys[KEY_W].down) { // up
    playerMoveUp(game->player);
  }
  if (keys[KEY_S].down) { // down
    playerMoveDown(game->player);
  }
  if (keys[KEY_SPACE].down && *bulletLimiter > 30.0f) {
    *bulletLimiter = 0.0f;
    addBullet(game->bullets, game->player->body.pos);
  } 

  printf("%f\n", *bulletLimiter);
  printf("%i\n", (int)game->bullets->aliveCount);
  // up current bullet limit counter
  *bulletLimiter += game->secondsPerFrame * 100;
}

void renderMap(Game *game) {
  printf("---- New Map ----\n");

  Vec2 roomMax = game->room->maxPos;
  Vec2 roomMin = game->room->minPos;
  Vec2 offset = roomMin;

  size_t viewX = (size_t)roundf(roomMax.x - roomMin.x + 1);
  size_t viewY = (size_t)roundf(roomMax.y - roomMin.y + 1);
  
  char mapBuffer[viewY][viewX];
  printf(
    "map=(%zu, %zu)\n",
    viewX - 1,
    viewY - 1
  );
  
  for (size_t y = 0; y < viewY; y++) {
    for (size_t x = 0; x < viewX; x++) {
      if (isPointInsideRoom(&game->room->shape, (Vec2){x,y}, offset)) {
        mapBuffer[y][x] = '-';
      }
      else {
        mapBuffer[y][x] = '#';
      }
    }
  }

  for (size_t e = 0; e < game->room->enemies->unitCount; e++) {
    Enemy enemy = game->room->enemies->units[e];
    
    placeUnit(viewX, mapBuffer, game, enemy);
  }

  for (int b = 0; b < MAX_BULLETS; b++){
    Bullet bullet = game->bullets->units[b];
    
    if (bullet.alive){   
      placeUnit(viewX, mapBuffer, game, bullet);
    }
  }
  
  placeUnit(viewX, mapBuffer, game, *game->player);

  for (size_t y = 0; y < viewY; y++) {
    for (size_t x = 0; x < viewX; x++) {
      printf(" %c", mapBuffer[y][x]);
    }
    printf("\n");
  }
};

void placeUnit(size_t maxX, char buffer[][maxX], Game *game, Unit unit){
  size_t arrayX = (size_t)roundf(unit.body.pos.x);
  size_t arrayY = (size_t)roundf(unit.body.pos.y);
      
  Vec2 worldPos = {
    (f32)arrayX,
    (f32)arrayY
  };
  
  if (isPointInsideRoom(&game->room->shape, worldPos, game->room->minPos)){
    buffer[arrayY][arrayX] = unit.symbol;
    printf(
      "unit map=(%f, %f)\n",
      unit.body.pos.x,
      unit.body.pos.y
    );
  }
}
