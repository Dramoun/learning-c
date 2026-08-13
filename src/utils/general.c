#include "utils/terminal_utils.h"
#include <time.h>


void sleepMs(long ms) {
  struct timespec ts;

  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000L;

  while (nanosleep(&ts, &ts) == -1) {
    if (shouldQuit()) {
      break;
    }
  }
}
