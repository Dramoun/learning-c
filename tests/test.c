#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#define _POSIX_C_SOURCE 199309L
#define RELEASE_DELAY 0.45

// ADDed
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

// ADDed
typedef struct {
  bool down;
  bool pressed;
  bool released;
} KeyState;

static struct termios original;

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &original);

  struct termios raw = original;
  raw.c_lflag &= ~(ICANON | ECHO);

  tcsetattr(STDIN_FILENO, TCSANOW, &raw);

  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

void disableRawMode() { tcsetattr(STDIN_FILENO, TCSANOW, &original); }

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

double now() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec + ts.tv_nsec * 1e-9;
}

void sleep_seconds(double seconds) {
  if (seconds <= 0) return;

  struct timespec ts;
  ts.tv_sec = (time_t)seconds;
 ts.tv_nsec = (long)((seconds - ts.tv_sec) * 1e9);

  nanosleep(&ts, NULL);
}

Key mapCharToKey(char c) {
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

// ADDed
void setKeyDown(Key key, KeyState keys[KEY_COUNT], double lastSeen[KEY_COUNT], double now) {
  if (key == KEY_UNKNOWN) {
    return;
  }

  if (!keys[key].down){
    keys[key].pressed = 1;
  }

  keys[key].down = 1;
  lastSeen[key] = now;
}

// ADDed
void handleEscapedSequences(double now, KeyState keys[KEY_COUNT], double lastSeen[KEY_COUNT]) {
  char seq[2];

  if (read(STDIN_FILENO, &seq[0], 1) != 1) {
    return;
  }
  
  if (read(STDIN_FILENO, &seq[1], 1) != 1) {
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
    }
  }
}

// ADDed
void updateInput(KeyState keys[KEY_COUNT], double lastSeen[KEY_COUNT]) {
  char c;
  double nowTime = now();

  while (read(STDIN_FILENO, &c, 1) == 1) {
    if (c == 27) {
      handleEscapedSequences(nowTime, keys, lastSeen);
    } else {
      Key k = mapCharToKey(c);
      setKeyDown(k, keys, lastSeen, nowTime);
    }
  }

  // Fake release
  for (int i = 0; i < KEY_COUNT;i++) {
    if (keys[i].down && (nowTime - lastSeen[i]) > RELEASE_DELAY) {
      keys[i].down = 0;
      keys[i].released = 1;
    }
  }
}

// aDDed
void beginInputFrame(KeyState keys[KEY_COUNT]) {
  for (int i = 0; i < KEY_COUNT; i++) {
    keys[i].pressed = 0;
    keys[i].released = 0;
  }
}

int main() {
  signal(SIGINT, handleSigint);
  atexit(disableRawMode);
  enableRawMode();

  KeyState keys[KEY_COUNT];
  double lastSeen[KEY_COUNT];
  
  // Fill them with default data
  for (int i = 0; i < KEY_COUNT; i++) {
    keys[i].down = 0;
    lastSeen[i] = 0;
  }

  const double dt = 1.0 / 60.0; // Update 60 times per 1t (60fps)
  double currentTime = now();
  double accumulator = 0.0;
  
  int frameCount = 0;
  int updateCountTotal = 0;
  double fpsTimer = now();

  int running = 1;
  
  while (running) {
    double frameStart = now();
    double frameTime = frameStart - currentTime;
  
    beginInputFrame(keys);
    updateInput(keys, lastSeen);

    currentTime = frameStart;
    
    // If there is a lag spike between last update and current update time
    // Minimize to X so we dont do insane amout of updates
    if (frameTime > 0.25){
      frameTime = 0.25;
    }

    accumulator += frameTime;
    
    // Max updates per cycle
    int updateCount = 0;
    const int maxUpdates = 5;

    while (accumulator >= dt && updateCount <= maxUpdates){
      // ----------------UPDATE HERE----------------
      if (keys[KEY_R].pressed) printf("R pressed\n");
      if (keys[KEY_R].released) printf("R released\n");
      if (keys[KEY_R].down) printf("R held\n");

      if (keys[KEY_UP].pressed) printf("UP pressed\n");

      if (keys[KEY_Q].pressed) running = 0;
      accumulator -= dt;
      updateCount++;
      updateCountTotal++;
    }
    
    // If we do max updates per cycle or accumulated time, then we reset so we dont do too many updates
    if (updateCount == maxUpdates){
      accumulator = 0;
    }

    double alpha = accumulator / dt;
    printf("RENDER alpha = %f\n", alpha);
    printf("Frame time: %f\n", frameTime);
    
    double elapsed = now() - frameStart;

    if (elapsed < dt){
      sleep_seconds(dt - elapsed);
    }

    frameCount++;

    double nowTime = now();
    
    // Every second
    if (nowTime - fpsTimer >= 1.0) {
        printf("\n=== STATS ===\n");
        printf("FPS (renders): %d\n", frameCount);
        printf("UPS (updates): %d\n", updateCountTotal);
        printf("=============\n\n");

        frameCount = 0;
        updateCountTotal = 0;
        fpsTimer = nowTime;
    }
  }
  
  disableRawMode();
  return 0;
}
