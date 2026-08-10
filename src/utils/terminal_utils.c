// For terminal usage funcs
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#include <time.h>

#include "terminal_utils.h"

/**
 * TEMP INPUT SYSTEM (to be replaced by SDL2)
 *
 * Usage:
 *
 * 1. Call enableSpecialTerminal() once at startup
 *
 * 2. In your main loop:
 *      beginInputFrame(keys);
 *      updateInput(keys, lastSeen);
 *
 * 3. Read input from keys[]:
 *      keys[KEY_X].pressed   -> triggered this frame
 *      keys[KEY_X].down      -> held
 *      keys[KEY_X].released  -> released this frame
 *
 * 4. On exit:
 *      disableSpecialTerminal();
 *
 * Notes:
 * - Requires a continuous loop (non-blocking input)
 * - Simulates key release using a timeout
 * - Will be replaced by SDL2 input handling
 */

#define RELEASE_DELAY 0.45

static volatile sig_atomic_t sigintReceived = 0;

static void enableRawMode();
static void disableRawMode();
static void handleSigint(int sig);

static Key mapCharToKey(char c);
static void setKeyDown(Key key, KeyState keys[KEY_COUNT], double lastSeen[KEY_COUNT], double now);
static void handleEscapedSequences(double now, KeyState keys[KEY_COUNT], double lastSeen[KEY_COUNT]);

static struct termios original;

static void enableRawMode() {
  tcgetattr(STDIN_FILENO, &original);

  struct termios raw = original;
  raw.c_lflag &= ~(ICANON | ECHO);

  tcsetattr(STDIN_FILENO, TCSANOW, &raw);

  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

static void handleSigint(int sig) {
  (void)sig;
  sigintReceived = 1;
}

bool shouldQuit() {
  return sigintReceived != 0;
}

static void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSANOW, &original);
}

void enableSpecialTerminal() {
  signal(SIGINT, handleSigint);
  atexit(disableRawMode);
  enableRawMode();
}

void disableSpecialTerminal() {
  disableRawMode();
}

static Key mapCharToKey(char c) {
  if (c >= 'a' && c <= 'z') {
    return (Key)(KEY_A + (c - 'a'));
  }
  if (c >= 'A' && c <= 'Z') {
    return (Key)(KEY_A + (c - 'A'));
  }

  if (c == 27) {
    return KEY_ESC;
  }

  if (c == '\n') {
    return KEY_ENTER;
  }

  if (c == ' ') {
    return KEY_SPACE;
  }

  return KEY_UNKNOWN;
}

static void setKeyDown(Key key, KeyState keys[KEY_COUNT], double lastSeen[KEY_COUNT], double now) {
  if (key == KEY_UNKNOWN) {
    return;
  }

  if (!keys[key].down){
    keys[key].pressed = 1;
  }

  keys[key].down = 1;
  lastSeen[key] = now;
}

static void handleEscapedSequences(
  double now,
  KeyState keys[KEY_COUNT],
  double lastSeen[KEY_COUNT]
) {
  char seq[2];

  // No second byte: standalone ESC
  if (read(STDIN_FILENO, &seq[0], 1) != 1) {
    setKeyDown(KEY_ESC, keys, lastSeen, now);
    return;
  }

  // No third byte: standalone/unknown ESC sequence
  if (read(STDIN_FILENO, &seq[1], 1) != 1) {
    setKeyDown(KEY_ESC, keys, lastSeen, now);
    return;
  }

  if (seq[0] == '[') {
    switch (seq[1]) {
      case 'A':
        setKeyDown(KEY_UP, keys, lastSeen, now);
        break;

      case 'B':
        setKeyDown(KEY_DOWN, keys, lastSeen, now);
        break;

      case 'C':
        setKeyDown(KEY_RIGHT, keys, lastSeen, now);
        break;

      case 'D':
        setKeyDown(KEY_LEFT, keys, lastSeen, now);
        break;

      default:
        setKeyDown(KEY_ESC, keys, lastSeen, now);
        break;
    }
  } else {
    setKeyDown(KEY_ESC, keys, lastSeen, now);
  }
}

void updateInput(KeyState keys[KEY_COUNT], double lastSeen[KEY_COUNT]) {
  char c;
  double now = timeNow();

  while (read(STDIN_FILENO, &c, 1) == 1) {
    if (c == 27) {
      handleEscapedSequences(now, keys, lastSeen);
    } else {
      Key k = mapCharToKey(c);
      setKeyDown(k, keys, lastSeen, now);
    }
  }

  // Fake release
  for (int i = 0; i < KEY_COUNT;i++) {
    if (keys[i].down && (now - lastSeen[i]) > RELEASE_DELAY) {
      keys[i].down = 0;
      keys[i].released = 1;
    }
  }
}

void beginInputFrame(KeyState keys[KEY_COUNT]) {
  for (int i = 0; i < KEY_COUNT; i++) {
    keys[i].pressed = 0;
    keys[i].released = 0;
  }
}

double timeNow() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec + ts.tv_nsec * 1e-9;
}

InitKeyPair getInitKeys() {
  InitKeyPair initKeyPair = {0};
  return initKeyPair;
}
