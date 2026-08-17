#ifndef UNITS_H
#define UNITS_H

#include "math/body.h"
#include "math/vec2.h"
#include "types.h"

#define MAX_BULLETS 100

typedef struct {
  char symbol;
  Body body;
  Vec2 defaultAcc;
  u16 hp;
  bool alive;
} Unit;

typedef Unit Bullet;
typedef Unit Enemy;
typedef Unit Player;

typedef struct {
  Bullet *units;
  size_t aliveCount;
  size_t deadCount;
} Bullets;

typedef struct {
  Enemy *units;
  size_t unitCount;
  size_t capacity;
} Enemies;

void addEnemy(Enemies *enemies, Enemy enemy); 
void removeEnemy(Enemies *enemies, size_t enemyIndex);
void addBullet(Bullets *bullets, Vec2 pos);
bool bulletEnemyCollision(Bullet *bullet, Enemy *enemy);

Enemies *createEnemies(size_t initialCapacity);
Bullets *createBulletSpace();

Player *createPlayer();
void playerMoveLeft(Player *player);
void playerMoveRight(Player *player);
void playerMoveDown(Player *player);
void playerMoveUp(Player *player);

#endif
