#include "components/structures.h"
#include "components/units.h"
#include "math/body.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>


void destroyRoom(Room *room) {
  if (!room) {
    return;
  }

  free(room->shape.vertices);

  if (room->enemies) {
    free(room->enemies->units);
    free(room->enemies);
  }
  
  free(room);
}

Room *roomOne() {
  size_t verticesCount = 12;

  Room *room = malloc(sizeof(Room));

  if (!room) {
      return NULL;
  }

  room->shape.verticesCount = verticesCount;
  room->shape.vertices = malloc(sizeof(Vec2) * verticesCount);

  if (!room->shape.vertices) {
    free(room);
    return NULL;
  }

  room->shape.vertices[0] = (Vec2){1,1};
  room->shape.vertices[1] = (Vec2){8,1};
  room->shape.vertices[2] = (Vec2){8,7};
  room->shape.vertices[3] = (Vec2){15,7};
  room->shape.vertices[4] = (Vec2){15,1};
  room->shape.vertices[5] = (Vec2){21,1};
  room->shape.vertices[6] = (Vec2){21,13};
  room->shape.vertices[7] = (Vec2){15,13};
  room->shape.vertices[8] = (Vec2){15,20};
  room->shape.vertices[9] = (Vec2){8,20};
  room->shape.vertices[10] = (Vec2){8,13};
  room->shape.vertices[11] = (Vec2){1,13};
 
  room->playerEntryPos = (Vec2){11, 19};
  
  // TODO: adding enemies also need to be checked and then destroying
  
  Enemies *enemies = createEnemies(5);

  if (!enemies) {
    free(room->shape.vertices);
    free(room);
    return NULL;
  }

  room->enemies = enemies;

  addEnemy(enemies, (Enemy){
    .symbol = 'E',
    .body = bodyCreate((Vec2){3, 3}, (Vec2){0, 0}), 
    .defaultAcc = {0, 0},
    .hp = 100,
    .alive = true
  });

  addEnemy(enemies, (Enemy){
    .symbol = 'E',
    .body = bodyCreate((Vec2) {5,5}, (Vec2) {0,0}),
    .defaultAcc = {0, 0},
    .hp = 100,
    .alive = true
  });

  addEnemy(enemies, (Enemy){
    .symbol = 'E',
    .body = bodyCreate((Vec2) {10,10}, (Vec2) {0,0}),
    .defaultAcc = {0, 0},
    .hp = 100,
    .alive = true
  });

  addEnemy(enemies, (Enemy){
    .symbol = 'E',
    .body = bodyCreate((Vec2) {16,5}, (Vec2) {0,0}),
    .defaultAcc = {0, 0},
    .hp = 100,
    .alive = true
  });

  addEnemy(enemies, (Enemy){
    .symbol = 'E',
    .body = bodyCreate((Vec2) {18,10}, (Vec2) {0,0}),
    .defaultAcc = {0, 0},
    .hp = 100,
    .alive = true
  });
  room->id = 1;

  calculateRoomBounds(room);
  return room;
}

void calculateRoomBounds(Room *room) {
  if (!room || !room->shape.vertices || room->shape.verticesCount == 0) {
    return;
  }

  Vec2 *vertices = room->shape.vertices;
  size_t count = room->shape.verticesCount;

  room->minPos = vertices[0];
  room->maxPos = vertices[0];

  for (size_t i = 1; i < count; i++) {
    if (vertices[i].x < room->minPos.x) {
      room->minPos.x = vertices[i].x;
    }

    if (vertices[i].y < room->minPos.y) {
      room->minPos.y = vertices[i].y;
    }

    if (vertices[i].x > room->maxPos.x) {
      room->maxPos.x = vertices[i].x;
    }

    if (vertices[i].y > room->maxPos.y) {
      room->maxPos.y = vertices[i].y;
    }
  }
}

bool isPointInsideRoom(RoomShape *roomShape, Vec2 p, Vec2 offset) {
  bool inside = false;

  Vec2 *vertices = roomShape->vertices;
  size_t a = roomShape->verticesCount - 1;

  for (size_t b = 0; b < roomShape->verticesCount; a = b++) {
    f64 ax = vertices[a].x - offset.x;
    f64 ay = vertices[a].y - offset.y;
    f64 bx = vertices[b].x - offset.x;
    f64 by = vertices[b].y - offset.y;

    // Check if p lies on the edge A -> B.
    f64 cross =
      (p.x - ax) * (by - ay) -
      (p.y - ay) * (bx - ax);

    if (fabs(cross) < 1e-9) {
      f64 minX = fmin(ax, bx);
      f64 maxX = fmax(ax, bx);
      f64 minY = fmin(ay, by);
      f64 maxY = fmax(ay, by);

      if (p.x >= minX && p.x <= maxX &&
        p.y >= minY && p.y <= maxY) {
        // True = on boundary is inside
        // False = on boundary is outside
        return false;
      }
    }

    // Normal ray-casting.
    bool crosses = (ay > p.y) != (by > p.y);

    if (crosses) {
      f64 ix = ax + ((p.y - ay) / (by - ay)) * (bx - ax);

      if (ix > p.x) {
        inside = !inside;
      }
    }
  }

  return inside;
}

void destroyGame(Game *game) {
  if (!game) {
    return;
  }

  destroyRoom(game->room);

  // TODO: custom destructor
  if (game->player) {
    free(game->player);
  }

  // TODO: custom destructor
  if (game->bullets){
    free(game->bullets->units);
    free(game->bullets);
  }

  free(game);
}

Game *createGame() {
  Game *game = malloc(sizeof(Game));

  if (!game) {
    return NULL;
  }

  game->room = roomOne();
  game->player = createPlayer();
  game->bullets = createBulletSpace();

  if (!game->room || !game->player || !game->bullets) {
    destroyRoom(game->room);
   
    // TODO: custom destructor
    if (game->player) {
      free(game->player);
    }

    // TODO: custom destructor
    if (game->bullets){
      free(game->bullets->units);
      free(game->bullets);
    }

    free(game);
    return NULL;
  }

  game->gameState = MAIN_MENU;
  game->secondsPerFrame = 0.016f;
  game->currentLevel = game->room->id;
  game->isRunning = 1;

  return game;
}
