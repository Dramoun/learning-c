// hello.c

#define _POSIX_C_SOURCE 199309L
#include "utils/terminal_utils.h"
#define MAX_BULLETS 100

#include <stdio.h>
#include <stdlib.h>

// For terminal usage funcs
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

//TODO: added vec2 and body so next we need units separated and using that stuff
enum GameState {
  MAIN_MENU,
  PLAYING,
  PAUSED,
  GAME_WIN,
  GAME_LOSE
};

struct Map {
  int maxX;
  int maxY;
  int minX;
  int minY;
};

struct Unit {
  char symbol;
  float x;  // Col
  float y;  // Row
  float vx; // Col velocity
  float vy; // Row velocity
  int hp;
};

struct Bullet {
  char symbol;
  float x;
  float y;
  float vx;
  float vy;
  int alive;
};

struct Bullets {
  struct Bullet *units;
  int aliveCount;
  int deadCount;
};

// Exponentially chunked
struct Enemies {
  struct Unit *units;
  int unitCount;
  int capacity;
};

struct Game {
  struct Map *map;
  struct Unit *player;
  struct Enemies *enemies;
  struct Bullets *bullets;
  enum GameState gameState;
  float secondsPerFrame;
  int currentLevel;
  int isRunning;
};

struct Map *createMap(int minX, int minY, int maxX, int maxY);
struct Unit *createPlayer(char symbol, float x, float y, int hp);
struct Enemies *createEnemies(int initialCapacity); // added
struct Game *createGame();
struct Bullets *createBulletSpace(); // added
void destroyGame(struct Game *game);

void updateUnitsPositions(struct Game *game);
void renderMap(struct Game *game); // added

void addEnemy(struct Enemies *enemies, struct Unit enemy); // added
void removeEnemy(struct Enemies *enemies, int enemyIndex); // added

void addBullet(struct Game *game, int x, int y, int vx, int vy); // added

void checkBuletPositions(struct Game *game);
void bulletBorderCollision(struct Game *game, struct Bullet *bullet);
int bulletEnemyCollision(struct Bullet *bullet, struct Unit *enemy); // added

void clampObjectsToBorders(struct Game *game);
void clampUnit(struct Game *game, struct Unit *unit);

void gameMenu(struct Game *game, KeyState keys[KEY_COUNT]);
void gamePlaying(struct Game *game, KeyState keys[KEY_COUNT], float *bulletLimiter);
void gamePaused(struct Game *game, KeyState keys[KEY_COUNT]);
void gameWin(struct Game *game, KeyState keys[KEY_COUNT]);
void gameLose(struct Game *game, KeyState keys[KEY_COUNT]);

// TODO: move this
void sleepMs(long ms) {
  struct timespec ts;

  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000L;

  while (nanosleep(&ts, &ts) == -1) {
    if (shouldQuit()) {
      break;
    }
  }
}

int main() {
  struct Game *game = createGame();
  float bulletLimiter = 0.0f;
  
  InitKeyPair keyPair = getInitKeys();

  if (!game) {
    printf("Failed to initialize game\n");
    return 1;
  }

  addEnemy(game->enemies, (struct Unit){'X', 1, 2, 0, 0, 2});
  addEnemy(game->enemies, (struct Unit){'X', 3, 2, 0, 0, 2});
  addEnemy(game->enemies, (struct Unit){'X', 5, 2, 0, 0, 2});
  addEnemy(game->enemies, (struct Unit){'X', 7, 2, 0, 0, 2});
  addEnemy(game->enemies, (struct Unit){'X', 9, 2, 0, 0, 2});

  renderMap(game);

  enableSpecialTerminal();

  double currentTime = timeNow();
  double accumulator = 0.0;
  const double dt = game->secondsPerFrame;

  while (game->isRunning && !shouldQuit()) {
    double frameStart = timeNow();
    double frameTime = frameStart - currentTime;
    currentTime = frameStart;
    
    if (frameTime > 0.25) {
      frameTime = 0.25;
    }

    beginInputFrame(keyPair.keys);
    updateInput(keyPair.keys, keyPair.lastSeen);

    accumulator += frameTime;
    int updateCount = 0;
    const int maxupdates = 5;
    /*
     * while (accumulator >= dt && updateCount < maxUpdates) {
     *   DO UPDATES HERE
     *   accumulator -= dt;
     *   updateCount++;
     * }
     *
     * if (updateCount == maxUpdates ) {
     *  accumulator = 0;
     * }
     * 
     * RENDER HERE
     *
     * double elapsed = timeNow() - frameStart;
     * 
     * if (elapsed < dt) {
     *   sleepMs( (dt - elapsed) * 1000)
     * }
     */

    switch (game->gameState){
      case MAIN_MENU:
        printf("MAIN MENU\n");
        gamePlaying(game, keyPair.keys, &bulletLimiter);
        //gameMenu(game, keyPair.keys);
        break;

      case PLAYING:
        printf("PLAYING\n");
        gamePlaying(game, keyPair.keys, &bulletLimiter);
        break;

      case PAUSED:
        printf("PAUSED\n");
        gamePaused(game, keyPair.keys);
        break;

      case GAME_WIN:
        printf("GAME WON\n");
        gameWin(game, keyPair.keys);
        break;

      case GAME_LOSE:
        printf("GAME LOST\n");
        gameLose(game, keyPair.keys);
        break;
    }
  }

  destroyGame(game);
  disableSpecialTerminal();

  return 0;
};

void gameMenu(struct Game *game, KeyState keys[KEY_COUNT]){
  if (keys[KEY_Q].pressed) {
    game->isRunning = 0;
  } else if (keys[KEY_P].pressed) { // play
    game->gameState = PLAYING;
  }
}

// TODO: need to separate update and render logics, we should be updateing more then once per frame
void gamePlaying(struct Game *game, KeyState keys[KEY_COUNT], float *bulletLimiter){
  if (keys[KEY_Q].pressed) {
    game->isRunning = 0;
    return;
  }
  
  game->player->vx = 0.0f;
  game->player->vy = 0.0f;
  
  if (keys[KEY_A].down) { // left
    game->player->vx = -30.0f;
  }
  if (keys[KEY_D].down) { // right
    game->player->vx = 30.0f;
  }
  if (keys[KEY_W].down) { // up
    game->player->vy = -30.0f;
  }
  if (keys[KEY_S].down) { // down
    game->player->vy = 30.0f;
  }
  if (keys[KEY_SPACE].down && *bulletLimiter > 30.0f) {
    *bulletLimiter = 0.0f;
    addBullet(game, game->player->x, game->player->y, 0.0f, -5.0f);
  } 

  updateUnitsPositions(game);
  checkBuletPositions(game);
  clampObjectsToBorders(game);
  printf("\033[H\033[J"); // clear screen
  renderMap(game);
  printf("%f\n", *bulletLimiter);
  printf("%i\n", game->bullets->aliveCount);
  // up current bullet limit counter
  *bulletLimiter += game->secondsPerFrame * 100;
  sleepMs(game->secondsPerFrame * 1000);
}

void gamePaused(struct Game *game, KeyState keys[KEY_COUNT]){
  
}

void gameWin(struct Game *game, KeyState keys[KEY_COUNT]){

}

void gameLose(struct Game *game, KeyState keys[KEY_COUNT]){

}

void renderMap(struct Game *game) {
  printf("---- New Map ----\n");

  int mapSizeX = game->map->maxX - game->map->minX;
  int mapSizeY = game->map->maxY - game->map->minY;
  char mapBuffer[mapSizeY][mapSizeX];

  for (int y = 0; y < mapSizeY; y++) {
    for (int x = 0; x < mapSizeX; x++) {
      mapBuffer[y][x] = '-';
    }
  }

  for (int e = 0; e < game->enemies->unitCount; e++) {
    struct Unit *enemy = &game->enemies->units[e];
    int arrayX = (int)enemy->x - game->map->minX;
    int arrayY = (int)enemy->y - game->map->minY;
    mapBuffer[arrayY][arrayX] = enemy->symbol;
  }

  for (int b = 0; b < MAX_BULLETS; b++){
    struct Bullet *bullet = &game->bullets->units[b];

    if (bullet->alive){
      int arrayX = (int)bullet->x - game->map->minX;
      int arrayY = (int)bullet->y - game->map->minY;
      mapBuffer[arrayY][arrayX] = bullet->symbol;
    }
  }
  
  int playerArrayX = (int)game->player->x - game->map->minX;
  int playerArrayY = (int)game->player->y - game->map->minY;
  mapBuffer[playerArrayY][playerArrayX] = game->player->symbol;

  for (int y = 0; y < mapSizeY; y++) {
    for (int x = 0; x < mapSizeX; x++) {
      printf(" %c", mapBuffer[y][x]);
    }
    printf("\n");
  }
};

void updateUnitsPositions(struct Game *game) {
  for (int i = 0; i < game->enemies->unitCount; i++) {
    game->enemies->units[i].x += game->enemies->units[i].vx * game->secondsPerFrame;
    game->enemies->units[i].y += game->enemies->units[i].vy * game->secondsPerFrame;
  }

  int bulletsMoved = 0;

  for (int i = 0; i < MAX_BULLETS; i++){
    if (bulletsMoved == game->bullets->aliveCount){
      break;
    }
    
    struct Bullet *bullet = &game->bullets->units[i];
    
    if (bullet->alive){
      bullet->x += bullet->vx * game->secondsPerFrame;
      bullet->y += bullet->vy * game->secondsPerFrame;
      bulletsMoved++;
    }
  }

  game->player->x += game->player->vx * game->secondsPerFrame;
  game->player->y += game->player->vy * game->secondsPerFrame;
}

void checkBuletPositions(struct Game *game){
  // bullet touched border == remove
  for (int i = 0; i < MAX_BULLETS; i++){
    struct Bullet *bullet = &game->bullets->units[i];

    if (bullet->alive){
      bulletBorderCollision(game, bullet);
    
      if (bullet->alive){
        // bullet touched enemy == remove ( bullet and enemy)
        for (int e = 0; e < game->enemies->unitCount; e++){
          struct Unit *enemy = &game->enemies->units[e];

          if (bulletEnemyCollision(bullet, enemy)){
            removeEnemy(game->enemies, e);
            game->bullets->aliveCount--;
            break;
          }
        }
      }
    }
  }
  // TODO: check if bullet touched player == remove (end game)
}

void bulletBorderCollision(struct Game *game, struct Bullet *bullet){
  int bulletRemoved = 0;

  if (bullet->x < game->map->minX){
    bullet->alive = 0;
    bulletRemoved = 1;
  } else if (bullet->x >= game->map->maxX){
    bullet->alive = 0;
    bulletRemoved = 1;
  } else if (bullet->y < game->map->minY){
    bullet->alive = 0;
    bulletRemoved = 1;
  } else if (bullet->y >= game->map->maxY){
    bullet->alive = 0;
    bulletRemoved = 1;
  }

  if (bulletRemoved){
    game->bullets->aliveCount--;
  }
}

int bulletEnemyCollision(struct Bullet *bullet, struct Unit *enemy){
  int enemyKilled = 0;
  
  if ((int)bullet->x == (int)enemy->x && (int)bullet->y == (int)enemy->y){
    bullet->alive = 0;
    enemyKilled = 1;
  }

  return enemyKilled;
}

void clampObjectsToBorders(struct Game *game) {
  for (int e = 0; e < game->enemies->unitCount; e++) {
    struct Unit *enemy = &game->enemies->units[e];
    clampUnit(game, enemy);
  }

  clampUnit(game, game->player);
}

void clampUnit(struct Game *game, struct Unit *unit) {
  if (unit->x < game->map->minX){
    unit->x = game->map->minX;
  }
  if (unit->x >= game->map->maxX){
    unit->x = game->map->maxX - 1;
  }
  if (unit->y < game->map->minY){
    unit->y = game->map->minY;
  }
  if (unit->y >= game->map->maxY){
    unit->y = game->map->maxY - 1;
  }
}

void addEnemy(struct Enemies *enemies, struct Unit enemy) {
  if (enemies->capacity <= enemies->unitCount) {
    int newCapacity = enemies->capacity * 2;

    struct Unit *temp =
        realloc(enemies->units, sizeof(struct Unit) * newCapacity);

    if (!temp) {
      printf("Memory realocation of enemies failed\n");
      return;
    }

    enemies->units = temp;
    enemies->capacity = newCapacity;
  };

  enemies->units[enemies->unitCount++] = enemy;
};

void removeEnemy(struct Enemies *enemies, int enemyIndex) {
  if (enemies->unitCount == 0) {
    return;
  };

  if (enemyIndex < 0 || enemyIndex >= enemies->unitCount) {
    printf("Trying to remove enemy with invalid index\n");
    return;
  };

  for (int i = enemyIndex; i < enemies->unitCount - 1; i++) {
    enemies->units[i] = enemies->units[i + 1];
  };
  enemies->unitCount--;
};

struct Map *createMap(int minX, int minY, int maxX, int maxY) {
  struct Map *map = malloc(sizeof(struct Map));

  if (!map) {
    return NULL;
  }

  map->minX = minX;
  map->minY = minY;
  map->maxX = maxX;
  map->maxY = maxY;

  return map;
}

struct Unit *createPlayer(char symbol, float x, float y, int hp) {
  struct Unit *player = malloc(sizeof(struct Unit));

  if (!player) {
    return NULL;
  }

  player->symbol = symbol;
  player->x = x;
  player->y = y;
  player->vx = 0;
  player->vy = 0;
  player->hp = hp;

  return player;
}

void addBullet(struct Game *game, int x, int y, int vx, int vy) {
  if (game->bullets->aliveCount == MAX_BULLETS){
    return;
  }
  
  for (int i = 0; i < MAX_BULLETS; i++){
    struct Bullet *bullet = &game->bullets->units[i];

    if (bullet->alive == 0){
      bullet->alive = 1;
      bullet->x = x;
      bullet->y = y;
      bullet->vx = vx;
      bullet->vy = vy;
      game->bullets->aliveCount++;
      return;
    }
  }

}

struct Enemies *createEnemies(int initialCapacity) {
  struct Enemies *enemies = malloc(sizeof(struct Enemies));

  if (!enemies) {
    return NULL;
  }

  enemies->units = malloc(sizeof(struct Unit) * initialCapacity);

  if (!enemies->units) {
    free(enemies);
    return NULL;
  }

  enemies->unitCount = 0;
  enemies->capacity = initialCapacity;

  return enemies;
}

struct Bullets *createBulletSpace(){
  struct Bullets *bullets = malloc(sizeof(struct Bullets));

  if (!bullets){
    return NULL;
  }

  struct Bullet *bulletSpace = malloc(sizeof(struct Bullet) * MAX_BULLETS); 

  if (!bulletSpace){
    free(bullets);
    return NULL;
  }
  
  for (int i = 0; i < MAX_BULLETS; i++){
    bulletSpace[i].alive = 0;
    bulletSpace[i].symbol = '|';
  }

  bullets->units = bulletSpace;
  bullets->aliveCount = 0;
  bullets->deadCount = 0;

  return bullets;
}

struct Game *createGame() {
  struct Game *game = malloc(sizeof(struct Game));

  if (!game) {
    return NULL;
  }

  game->map = createMap(0, 0, 10, 10);
  game->player = createPlayer('P', 4, 9, 10);
  game->enemies = createEnemies(4);
  game->bullets = createBulletSpace();

  if (!game->map || !game->player || !game->enemies || !game->bullets) {
    free(game->map);
    free(game->player);

    if (game->enemies) {
      free(game->enemies->units);
      free(game->enemies);
    }

    if (game->bullets){
      free(game->bullets->units);
      free(game->bullets);
    }

    free(game);
    return NULL;
  }

  game->gameState = MAIN_MENU;
  game->secondsPerFrame = 0.016f;
  game->currentLevel = 0;
  game->isRunning = 1;

  return game;
}

void destroyGame(struct Game *game) {
  if (!game) {
    return;
  }

  free(game->map);
  free(game->player);

  if (game->enemies) {
    free(game->enemies->units);
    free(game->enemies);
  }

  if (game->bullets) {
    free(game->bullets->units);
    free(game->bullets);
  }

  free(game);
}

