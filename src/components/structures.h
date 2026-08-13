#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "types.h"
#include "units.h"
#include <stddef.h>


typedef enum {
  MAIN_MENU,
  PLAYING,
  PAUSED,
  GAME_WIN,
  GAME_LOSE
} GameState;

typedef struct {
  Vec2 *vertices;
  size_t verticesCount;
} RoomShape;

typedef struct {
  Enemies *enemies;
  RoomShape shape;
  Vec2 playerEntryPos;
  u8 id;
} Room;

typedef struct {
  Room *room;
  Unit *player;
  Bullets *bullets;
  GameState gameState;
  f32 secondsPerFrame;
  u8 currentLevel;
  bool isRunning;
} Game;

Game *createGame();
void destroyGame(Game *game);

Room *roomOne();
void destroyRoom(Room *room);

#endif
