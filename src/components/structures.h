#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdbool.h>
#include "types.h"
#include "units.h"


typedef enum {
  MAIN_MENU,
  PLAYING,
  PAUSED,
  GAME_WIN,
  GAME_LOSE
} GameState;

typedef struct {
  u8 maxX;
  u8 maxY;
  u8 minX;
  u8 minY;
} MapSize;

typedef struct {
  MapSize *map;
  Unit *player;
  Enemies *enemies;
  Bullets *bullets;
  GameState gameState;
  f32 secondsPerFrame;
  u8 currentLevel;
  bool isRunning;
} Game;

#endif
