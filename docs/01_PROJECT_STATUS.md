# Project Status

*A snapshot from a code review on 2026-07-27 — this describes where the project stood when this was written, not a living doc that updates itself as you keep coding.*

## The one-line version

A real-time, terminal-rendered 2D action game written in C. You control `P`, move with WASD, shoot with Space, and fight a handful of enemies on a small grid. Right now it plays a lot like a stripped-down Space Invaders — a fixed row of enemies near the top, you starting near the bottom, shooting upward — but the underlying systems (free 8-directional movement, velocity on everything, a real object pool for bullets) are clearly built for something more open-ended than that.

There's no working menu, pause, win, or lose screen yet. Functionally, there's one mode: play until you press `Q`.

Just under 1,000 lines across the whole project right now (971, to be exact) — 574 of them still in `main.c`.

## At a glance

| File | Lines | Role | Status |
|---|---|---|---|
| `main.c` | 574 | `Game` struct, state machine, gameplay logic, collision, rendering, create/destroy lifecycle | Working, monolithic — this is genuinely everything right now |
| `terminal_utils.h` / `.c` | 46 / 183 | Hand-rolled non-blocking keyboard input, standing in for SDL2 | Working |
| `types.h` | 27 | Shared type aliases (`f32`, `i32`, `u8`, ...) and an unused `String` struct | Working, lightly used |
| `vec2.h` / `.c` | 20 / 41 | 2D vector math: add, sub, scale, length, normalize | Working, correct — not used anywhere yet |
| `body.h` / `.c` | 18 / 30 | A `Body` (pos/vel/acc) with proper force accumulation | Working, correct — not used anywhere yet |
| `units.h` / `.c` | 32 / 0 | An in-progress pull of `Unit`/`Bullet`/`Bullets`/`Enemies` out of `main.c` | Incomplete, not wired into anything yet |

Your own comment in `main.c` — `// TODO: added vec2 and body so next we need units separated and using that stuff` — is a completely accurate summary. `vec2`/`body` are done and ready to use. `units` is a half-finished move (the `.c` file is literally 0 bytes). Nothing has adopted either yet.

## What the loop actually does, right now

Straight from `main.c`:

1. `createGame()` builds a `Game`: a 10×10 `Map`, a player `Unit` (`'P'`, starting at x=4, y=9, 10 HP), an `Enemies` array, and a `Bullets` pool.
2. Five enemies (`'X'`, 2 HP each) are placed in a fixed row at y=2, not moving (`vx=vy=0`).
3. Raw terminal mode goes on (`enableSpecialTerminal`) — this is what lets you read keys without the terminal waiting for Enter, and without them echoing to the screen.
4. Then, every frame, until `isRunning` goes false:
   - clear the screen (ANSI escape)
   - `beginInputFrame` resets the per-frame `pressed`/`released` flags
   - `updateInput` drains whatever bytes are waiting on stdin (non-blocking), turns them into `Key` states, and fakes "key released" via a ~450ms timeout (`RELEASE_DELAY`) since a plain terminal read gives you no real key-up event
   - a `switch` on `gameState` routes to a handler — right now `MAIN_MENU` **and** `PLAYING` both call `gamePlaying()` directly
   - inside `gamePlaying()`: read WASD/Space into player velocity → move everyone → resolve bullet collisions → clamp everyone to the map edge → render → sleep ~16ms
5. `Q` sets `isRunning = 0` mid-frame, which unwinds the loop, frees everything, and restores the terminal.

## Controls, as actually wired today

| Key | Effect |
|---|---|
| `W` / `A` / `S` / `D` | Move up / left / down / right |
| `Space` (held) | Fire upward once the cooldown clears (~0.3s between shots) |
| `Q` | Quit |
| Arrow keys | Recognized by the input layer, not consumed by gameplay yet |
| `Esc` | Recognized in theory, but see `03_KNOWN_ISSUES.md` — pressing it alone currently does nothing |

## What's genuinely solid

Worth naming, since a status check like this can read as a list of gaps otherwise:

- **Allocation-failure handling is careful.** `createGame()`/`destroyGame()` and each individual `create*` function check every `malloc`/`realloc` and clean up partial failures correctly, with no double-frees on the paths they handle. That's a habit a lot of people skip.
- **The bullet pool is a real object pool.** `createBulletSpace` preallocates `MAX_BULLETS` (100) bullets once, and reuses dead slots via the `alive` flag instead of `realloc`-ing every shot. That's exactly what Part 3's "Object Pooling" exercise was asking for.
- **`vec2`/`body` are clean and correct**, and already use a better integration method (semi-implicit Euler: velocity updates first, then position uses the *new* velocity) than the raw float math currently used directly in `main.c`.
- **The non-blocking input system works.** Getting real-time, non-blocking keyboard input in raw C on Linux is genuinely fiddly, and this does it correctly (`termios` + `O_NONBLOCK`, polled every frame).

## What's visibly a stub

- `MAIN_MENU` doesn't behave like a menu — it calls the same `gamePlaying()` as `PLAYING`, and the real `gameMenu(...)` call sits commented out right above it.
- `PAUSED`, `GAME_WIN`, and `GAME_LOSE` are currently unreachable — nothing in the code ever sets `gameState` to any of them — and their handler functions are empty. If you did land in one today, the game would just print that label forever with no way out.
- There's exactly one wave of five stationary enemies, placed once at startup. No progression, no respawns, no difficulty curve.
- `hp` exists on both the player and enemies, but nothing decrements it — one bullet touch removes an enemy outright regardless of its `hp`, and nothing currently damages the player at all. So there's no real win/lose condition reachable yet.
- Timing is a flat assumed 16ms/frame plus a `sleep`, not a measured, accumulator-driven timestep.

See `03_KNOWN_ISSUES.md` for the specific bugs behind a few of these, and `02_ROADMAP.md` for what's actually next.

## Building it

Based on `main.c`'s `#include "utils/terminal_utils.h"`, your real folder layout puts `terminal_utils.*` inside a `utils/` subfolder next to `main.c`. Something along these lines (adjust paths to wherever `vec2`/`body`/`units` actually live relative to `main.c` on your machine):

```bash
gcc main.c utils/terminal_utils.c vec2.c body.c -o game -lm
```

`-lm` is needed because `vec2.c` calls `sqrtf`. `units.c` is currently empty, so there's nothing to gain by compiling it yet — add it to this line once you move real code into it.

(Small fossil, for what it's worth: `main.c`'s own header comment still reads `// hello.c` — and coincidentally, that's also the exact filename used in Part 4's own Valgrind walkthrough.)
