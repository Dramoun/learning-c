#include "components/units.h"
#include "math/body.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: prints should be logging into a logger file
void addEnemy(Enemies *enemies, Enemy enemy) {
  if (enemies->capacity <= enemies->unitCount) {
    size_t newCapacity = enemies->capacity * 2;

    Enemy *temp =
        realloc(enemies->units, sizeof(Enemy) * newCapacity);

    if (!temp) {
      printf("Memory realocation of enemies failed\n");
      return;
    }

    enemies->units = temp;
    enemies->capacity = newCapacity;
  };

  enemies->units[enemies->unitCount++] = enemy;
};

void removeEnemy(Enemies *enemies, size_t enemyIndex) {
  if (enemies->unitCount == 0) {
    return;
  };

  if (enemyIndex < 0 || enemyIndex >= enemies->unitCount) {
    printf("Trying to remove enemy with invalid index\n");
    return;
  };

  for (size_t i = enemyIndex; i < enemies->unitCount - 1; i++) {
    enemies->units[i] = enemies->units[i + 1];
  };
  enemies->unitCount--;
};

void addBullet(Bullets *bullets, Vec2 pos, Vec2 vel, Vec2 acc) {
  if (bullets->aliveCount == MAX_BULLETS){
    return;
  }
  
  for (int i = 0; i < MAX_BULLETS; i++){
    Bullet *bullet = &bullets->units[i];

    if (bullet->alive == false){
      bullet->alive = true;
      bullet->body = (Body){ 
        .pos=pos,
        .vel=vel,
        .acc=acc,
        .hitbox=0.5
      };

      bullets->aliveCount++;
      return;
    }
  }
}

bool bulletEnemyCollision(Bullet *bullet, Enemy *enemy){
  return twoBodyCollisionCheck(&bullet->body, &enemy->body);  
}

Enemies *createEnemies(size_t initialCapacity) {
  Enemies *enemies = malloc(sizeof(Enemies));

  if (!enemies) {
    return NULL;
  }

  enemies->units = malloc(sizeof(Enemy) * initialCapacity);

  if (!enemies->units) {
    free(enemies);
    return NULL;
  }

  enemies->unitCount = 0;
  enemies->capacity = initialCapacity;

  return enemies;
}

Bullets *createBulletSpace(){
  Bullets *bullets = malloc(sizeof(Bullets));

  if (!bullets){
    return NULL;
  }

  Bullet *bulletSpace = malloc(sizeof(Bullet) * MAX_BULLETS); 

  if (!bulletSpace){
    free(bullets);
    return NULL;
  }
  
  for (int i = 0; i < MAX_BULLETS; i++){
    bulletSpace[i].body = bodyCreate((Vec2){4,9}, (Vec2){0,0});
    bulletSpace[i].hp = 0;
    bulletSpace[i].defaultAcc = (Vec2) {1,0};
    bulletSpace[i].alive = 0;
    bulletSpace[i].symbol = '|';
  }

  bullets->units = bulletSpace;
  bullets->aliveCount = 0;
  bullets->deadCount = MAX_BULLETS;

  return bullets;
}

Player *createPlayer() {
  Player *player = malloc(sizeof(Player));

  if (!player) {
    return NULL;
  }

  player->symbol = 'P';
  player->body = bodyCreate((Vec2){4,9}, (Vec2){0,0});

  player->defaultAcc.x = 0;
  player->defaultAcc.y = 0;
  
  player->hp = 10;

  return player;
}
