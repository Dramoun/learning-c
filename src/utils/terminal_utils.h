#ifndef TERMINAL_UTILS_H
#define TERMINAL_UTILS_H

#include <stdbool.h>

typedef enum {
  KEY_UNKNOWN = 0,
  KEY_A, KEY_B, KEY_C, KEY_D, KEY_E,
  KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
  KEY_K, KEY_L, KEY_M, KEY_N, KEY_O,
  KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
  KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,

  KEY_ESC,
  KEY_ENTER,
  KEY_SPACE,

  KEY_UP,
  KEY_DOWN,
  KEY_LEFT,
  KEY_RIGHT,

  KEY_COUNT
} Key;

typedef struct {
  bool down;
  bool pressed;
  bool released;
} KeyState;

typedef struct {
  KeyState keys[KEY_COUNT];
  double lastSeen[KEY_COUNT];
} InitKeyPair;

void enableSpecialTerminal();
void disableSpecialTerminal();
bool shouldQuit();

double timeNow();

void beginInputFrame(KeyState keys[KEY_COUNT]);
void updateInput(KeyState keys[KEY_COUNT], double lastSeen[KEY_COUNT]);
InitKeyPair getInitKeys();

#endif
