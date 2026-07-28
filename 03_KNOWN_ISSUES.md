# Known Issues

*18 things found while reading through the code for this review, sorted into four buckets by how much they actually matter — not a verdict on the code as a whole. `createGame()`/`destroyGame()` alone show more careful allocation-failure handling than a lot of people bother with at this stage.*

## A. Could actually bite you (memory-safety issues)

### A1. `addEnemy()` can corrupt memory if a `realloc` ever fails

```c
void addEnemy(struct Enemies *enemies, struct Unit enemy) {
  if (enemies->capacity <= enemies->unitCount) {
    enemies->capacity = enemies->capacity * 2;

    struct Unit *temp =
        realloc(enemies->units, sizeof(struct Unit) * (enemies->capacity));

    if (!temp) {
      printf("Memory realocation of enemies failed\n");
      return;
    }

    enemies->units = temp;
  };

  enemies->units[enemies->unitCount++] = enemy;
};
```

`enemies->capacity` is doubled *before* `realloc` is attempted. If `realloc` fails, the function prints a message and returns — but `capacity` is left holding the doubled value even though `enemies->units` still points at the old, smaller block. The next call to `addEnemy` will trust the wrong `capacity`, believe there's room, and write past the end of the real allocation.

In practice, `realloc` failing on an allocation this small is rare — which is exactly why this kind of bug tends to survive unnoticed. Fix shape: compute the candidate size into a local variable first, and only assign it into `enemies->capacity` after confirming `temp` isn't `NULL`.

### A2. Ctrl+C skips `destroyGame()` entirely

```c
static void handleSigint(int sig) {
  (void)sig;
  disableRawMode();
  exit(0);
}
```

Compare to the normal shutdown path in `main()`:

```c
  destroyGame(game);
  disableSpecialTerminal();
  return 0;
```

`SIGINT` (Ctrl+C) restores the terminal and calls `exit(0)` directly — it never touches `game`, so the map, player, enemies array, bullets array, and the `Game` struct itself are all left allocated when the process exits this way. This is the exact scenario Part 4's memory-audit exercise asks you to go test ("What happens if you quit mid-game? (Ctrl+C path)") — now you know the answer going in.

Fix shape: give the handler a way to reach the `Game*` (a module-level pointer set once in `main`, for instance), or have it set a flag and let the normal loop exit and call `destroyGame()` as usual, instead of calling `exit()` from inside the handler.

### A3. `gameMenu()` indexes the key array out of bounds

```c
void gameMenu(struct Game *game, KeyState keys[KEY_COUNT]){
  if (keys['q'].pressed) {
    game->isRunning = 0;
  } else if (keys['p'].pressed) { // play
    game->gameState = PLAYING;
  }
}
```

`KEY_COUNT` is 34, so `keys[KEY_COUNT]` only has valid indices 0–33. `'q'` and `'p'` are their ASCII values — 113 and 112 — nowhere close to that range. This reads memory outside the array.

It's harmless *today* only because `gameMenu()` is never actually called — in `main()`, the `MAIN_MENU` case calls `gamePlaying()` directly, and the real `gameMenu(...)` call sits commented out right above it. This needs fixing before that call gets re-enabled. The correct pattern is already used elsewhere in the same file, in `gamePlaying()`:

```c
  if (keys[KEY_Q].pressed) {
```

Same fix here: `keys[KEY_Q]` and `keys[KEY_P]`.

## B. Logic bugs (wrong behavior, won't crash)

### B1. A standalone Escape key press is never registered

```c
while (read(STDIN_FILENO, &c, 1) == 1) {
    if (c == 27) {
      handleEscapedSequences(now, keys, lastSeen);
    } else {
      Key k = mapCharToKey(c);
      setKeyDown(k, keys, lastSeen, now);
    }
}
```

```c
static void handleEscapedSequences(double now, KeyState keys[KEY_COUNT], double lastSeen[KEY_COUNT]) {
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
```

Every byte `27` gets routed into `handleEscapedSequences`, which only recognizes 3-byte arrow-key sequences (`ESC [ A/B/C/D`) and returns if the next two bytes don't show up. `mapCharToKey` does correctly map a bare `27` to `KEY_ESC` elsewhere in this file — but that path is never reached for a standalone Escape press, since `updateInput` always diverts `27` into the arrow-sequence handler first. Matters as soon as you want Escape to do anything (back out of a menu, pause).

Fix shape: if the two follow-up reads in `handleEscapedSequences` don't complete an arrow sequence, fall through to registering `KEY_ESC` instead of silently returning.

### B2. Collision uses truncated-integer equality instead of distance

```c
int bulletEnemyCollision(struct Bullet *bullet, struct Unit *enemy) {
  int enemyKilled = 0;
  if ((int)bullet->x == (int)enemy->x && (int)bullet->y == (int)enemy->y){
    bullet->alive = 0;
    enemyKilled = 1;
  }
  return enemyKilled;
}
```

Truncating both coordinates to `int` before comparing means two points that are genuinely close can be missed (`1.9` vs `2.1` — only 0.2 apart, but truncate to `1` and `2` → no hit), while two points nearly a full unit apart, but on the same side of a boundary, register as a hit (`2.1` vs `2.9` → both truncate to `2`). This is precisely the trap Part 3, Exercise 8 calls out — worth revisiting with an `abs(dx) < 1 && abs(dy) < 1`-style check instead, as that exercise suggests.

### B3. `addBullet`'s parameter types don't match the struct it fills in

```c
void addBullet(struct Game *game, int x, int y, int vx, int vy) {
```

called as:

```c
addBullet(game, game->player->x, game->player->y, 0.0f, -5.0f);
```

`struct Bullet` stores `x`/`y`/`vx`/`vy` as `float`, but `addBullet`'s parameters are `int` — so the player's (possibly fractional) position truncates on the way in, and any non-whole velocity you pass later would silently truncate too. Fix shape: change the parameter types to `float` (or `f32`, matching `types.h`) so values round-trip exactly.

### B4. `deadCount` is never actually used

```c
struct Bullets {
  struct Bullet *units;
  int aliveCount;
  int deadCount;
};
```

Set to `0` in `createBulletSpace` and never read or written anywhere else. Either wire it up (increment it somewhere meaningful, e.g. when a bullet dies) or drop it for now — as it stands it quietly implies tracking that isn't happening.

### B5. `hp` exists but nothing consumes it

Enemies are created with `hp = 2` (see the `addEnemy` calls in `main()`), and the player has `hp = 10`, but nothing anywhere decrements either. `bulletEnemyCollision` removes an enemy on any touch regardless of `hp`, and nothing currently damages the player. Not "wrong," since combat/waves haven't been built yet — just flagging it so it's not a surprise later that `hp` is currently decorative.

## C. Incomplete / expected at this stage — not really bugs

These are all things you already know are unfinished (several match your own TODO comment directly) — listed so nothing looks mysterious when you come back to it.

### C1. `units.h` has no include guard

```c
#include <stdbool.h>

struct Unit {
```

Every other header in the project — `types.h`, `vec2.h`, `body.h`, `terminal_utils.h` — has an `#ifndef`/`#define`/`#endif` guard. `units.h` is the one exception. Harmless today only because nothing `#include`s it yet; add the guard before wiring it in, or the moment it's included from more than one place you'll get a "redefinition of struct Unit" error.

### C2. `units.h`/`units.c` duplicate, rather than replace, the structs in `main.c`

`units.h` currently repeats the same `Unit`/`Bullet`/`Bullets`/`Enemies` definitions that still live directly in `main.c`. `units.c` is empty (0 bytes). Nothing `#include`s `units.h` yet. This is a move in progress, not a bug — matches your own TODO comment exactly.

### C3. `Vec2`/`Body` aren't used by gameplay yet

`Unit`/`Bullet` in `main.c` still use raw `float x, y, vx, vy` rather than a `Body`. Also matches the TODO comment — `vec2`/`body` are ready, just not adopted yet.

### C4. Timestep is a flat constant + sleep, not a real accumulator

This is precisely what Part 4, Exercise 2 is about — noted here mainly so it's clear this is a known, planned gap and not an oversight.

### C5. `PAUSED`/`GAME_WIN`/`GAME_LOSE` are unreachable, and `MAIN_MENU` bypasses `gameMenu()`

Expected — this is Part 4, Exercise 1's job to finish.

## D. Minor & cosmetic

- **D1 — stray `;` after several closing braces.** `main()`, `renderMap()`, `addEnemy()`, and `removeEnemy()` all end with an extra `};` — a harmless empty statement, but some compilers flag it under stricter `-Wpedantic`-style settings.
- **D2 — `disableRawMode` doesn't restore the `O_NONBLOCK` flag.** `enableRawMode` sets stdin non-blocking via `fcntl`, but `disableRawMode` only restores the `termios` struct, not the file status flags. Worth saving/restoring both, so the shell's stdin isn't left non-blocking after the game exits.
- **D3 — "Bulet" vs "Bullet."** `checkBuletPositions` and friends — a consistent typo, purely cosmetic, doesn't affect behavior.
- **D4 — `isRunning` is `int`, not `bool`.** `<stdbool.h>` is already available transitively (via `terminal_utils.h`); could use `bool` for consistency. Same applies to `Bullet.alive`.
- **D5 — `units.h` includes `<stdbool.h>` but doesn't use `bool`,** and unlike `vec2.h`/`body.h`, doesn't use the `f32`/`i32`-style typedefs from `types.h` for its fields. Worth aligning once you wire it in.
- **D6 — `main.c` includes `<fcntl.h>`, `<termios.h>`, and `<unistd.h>` directly** but doesn't appear to call anything from them — those calls all live in `terminal_utils.c` now. Likely left over from before the terminal code was split out; safe to trim.
- **D7 — `terminal_utils.c` calls `clock_gettime(CLOCK_MONOTONIC, ...)` but doesn't define `_POSIX_C_SOURCE 199309L`** before its system includes, unlike `main.c`. Feature-test macros are per-file, so `main.c`'s define doesn't help here. Works today most likely because GCC's default mode exposes `clock_gettime` anyway — worth adding to `terminal_utils.c` too for consistency if you ever build with a stricter `-std=` flag.
