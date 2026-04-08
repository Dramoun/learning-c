//hello.c

// For time usage funcs on linux
#define _POSIX_C_SOURCE 199309L
#include <time.h>

#include <stdio.h>
#include <stdlib.h>

// For terminal usage funcs
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

static struct termios original;

struct Map { 
  int maxX;
  int maxY;
  int minX;
  int minY;
};

struct Unit {
  char symbol;
  float x; // Col
  float y; // Row
  float vx; // Col velocity
  float vy; // Row velocity
  int hp;
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
  float secondsPerFrame;
  int currentLevel;
  int isRunning;
};

// -- For save usage of terminal with better button presses then event blocking input
void enableRawMode();
void disableRawMode();
void handleSigint();
char readKey();
int keyPressed();
// --
//
// Sleep func - check this out later
void sleepMs(
    long ms
);

struct Map *createMap(int minX, int minY, int maxX, int maxY);
struct Unit *createPlayer(char symbol, float x, float y, int hp);
struct Enemies *createEnemies(int initialCapacity);
struct Game *createGame();
void destroyGame(struct Game *game);

void updateUnitsPositions(struct Game *game);
int unitInBounds(struct Game *game, struct Unit *unit);
void renderMap(struct Game *game);

void addEnemy(struct Enemies *enemies, struct Unit enemy);
void removeEnemy(struct Enemies *enemies, int enemyIndex);

int checkCollision(int cx, int cy, int tx, int ty);

int main(){
  struct Game *game = createGame();

  if (!game){
    printf("Failed to initialize game\n");
    return 1;
  }

  // Appearing, wanna reprint here?
  addEnemy(game->enemies, (struct Unit){'X', 1, 2, 0, 0, 2});
  addEnemy(game->enemies, (struct Unit){'X', 3, 2, 0, 0, 2});
  addEnemy(game->enemies, (struct Unit){'X', 5, 2, 0, 0, 2});
  addEnemy(game->enemies, (struct Unit){'X', 7, 2, 0, 0, 2});
  addEnemy(game->enemies, (struct Unit){'X', 9, 2, 0, 0, 2});

  renderMap(game);

  signal(SIGINT, handleSigint);
  atexit(disableRawMode);
  enableRawMode();

  while(game->isRunning){
    printf("\033[H\033[J"); // clear screen

    char userInput = readKey();  

    if (userInput == 'q'){
      game->isRunning = 0;
      continue;
    } else if (userInput == 'a'){ //left 
      game->player->vx = -5.0f;
    } else if (userInput == 'd'){ //right
      game->player->vx = 5.0f;
    } else if (userInput == 'w'){ //up
      game->player->vy = -5.0f;
    } else if (userInput == 's'){ //down
      game->player->vy = 5.0f;
    } else {
      game->player->vx = 0.0f;
      game->player->vy = 0.0f;
    }

    //TODO: colision checks missing
    updateUnitsPositions(game);
    renderMap(game);

    sleepMs(game->secondsPerFrame * 1000);
  }
  
  destroyGame(game);
  disableRawMode();

  return 0;
};

void renderMap(struct Game *game) {
  printf("---- New Map ----\n");

  for (int y = game->map->minY; y < game->map->maxY; y++) {
    for (int x = game->map->minX; x < game->map->maxX; x++) {

      int enemyAdded = 0;
      for (int i = 0; i < game->enemies->unitCount; i++) {
        struct Unit *enemy = &game->enemies->units[i];

        if (enemy->y == y && enemy->x == x) {
          printf(" %c", enemy->symbol);
          enemyAdded = 1;
          break;
        }
      }

      if (!enemyAdded && game->player->y == y && game->player->x == x) {
        printf(" %c", game->player->symbol);
      } else if (!enemyAdded) {
        printf(" -");
      }
    };
    printf("\n");
  };
};

void updateUnitsPositions(struct Game *game) {
  for (int i = 0; i < game->enemies->unitCount; i++) {
    if (unitInBounds(game, &game->enemies->units[i])){
      game->enemies->units[i].x += game->enemies->units[i].vx * game->secondsPerFrame;
      game->enemies->units[i].y += game->enemies->units[i].vy * game->secondsPerFrame;
    }
  }
  
  if (unitInBounds(game, game->player)){
    game->player->x += game->player->vx * game->secondsPerFrame;
    game->player->y += game->player->vy * game->secondsPerFrame;
  }
}

int unitInBounds(struct Game *game, struct Unit *unit){
  if (unit->y < game->map->minY || unit->y >= game->map->maxY ||
      unit->x < game->map->minX || unit->x >= game->map->maxX) {
    return 0;
  };

  return 1;
}

void addEnemy(struct Enemies *enemies, struct Unit enemy) {
  if (enemies->capacity <= enemies->unitCount) {
    enemies->capacity = enemies->capacity * 2;

    struct Unit *temp =
        realloc(enemies->units, sizeof(struct Unit) * (enemies->capacity));

    if (!temp) {
      printf("Memory realocation of enemies failed\n");
      return;
    }

    enemies->units = temp;
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

int checkCollision(int cx, int cy, int tx, int ty) {
  if (cx == tx && cy == ty) {
    return 1;
  }

  return 0;
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

struct Game *createGame() {
  struct Game *game = malloc(sizeof(struct Game));

  if (!game) {
    return NULL;
  }

  game->map = createMap(0, 0, 10, 10);
  game->player = createPlayer('P', 4, 9, 10);
  game->enemies = createEnemies(4);

  if (!game->map || !game->player || !game->enemies) {
    free(game->map);
    free(game->player);

    if (game->enemies) {
      free(game->enemies->units);
      free(game->enemies);
    }

    free(game);
    return NULL;
  }
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

  free(game);
}

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &original);

  struct termios raw = original;
  raw.c_lflag &= ~(ICANON | ECHO);

  tcsetattr(STDIN_FILENO, TCSANOW, &raw);

  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

void disableRawMode() { tcsetattr(STDIN_FILENO, TCSANOW, &original); }

int keyPressed() {
  char ch;
  return read(STDIN_FILENO, &ch, 1) == 1;
}

char readKey() {
  char ch;
  if (read(STDIN_FILENO, &ch, 1) == 1)
    return ch;
  return 0;
}

void sleepMs(long ms) {
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000;
  nanosleep(&ts, NULL);
}

void handleSigint() {
  disableRawMode();
  exit(0);
}
