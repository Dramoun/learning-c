#include <stdbool.h>

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
