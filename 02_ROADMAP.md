# Roadmap — Where This Is Headed

*Companion to `01_PROJECT_STATUS.md`. Written against the two roadmap documents you've been working from — "Part 3: Real-Time Loop, Input, and Simulation" (essentially done) and "Part 4: Architecture & State Management" (next, and lightly started).*

## Part 3 — audit

| Exercise | Status | Note |
|---|---|---|
| 1. Non-blocking input | ✅ Done | `termios` raw mode + `O_NONBLOCK`, polled every frame in `terminal_utils.c` |
| 2. Frame-based loop, fixed dt | ✅ Done (basic form) | `secondsPerFrame = 0.016f` drives all movement; the loop paces itself with `sleepMs`. Not yet a *measured*, accumulator-driven timestep — that's Part 4, Exercise 2 |
| 3. Rendering without flicker | ✅ Done | ANSI clear + a `mapBuffer[][]` filled once and printed once per `renderMap()` |
| 4. Velocity-based movement | ✅ Done | `vx`/`vy` on `Unit` and `Bullet`, integrated in `updateUnitsPositions()` |
| 5. Boundaries & constraints | ✅ Done | Units clamp to the edge (`clampUnit`); bullets die on leaving the map (`bulletBorderCollision`) rather than bouncing — a reasonable design choice you made |
| 6. Bullets / temporary objects | ✅ Done | `Bullet` + `alive` flag, spawned via `addBullet` |
| 7. Object pooling | ✅ Done | `createBulletSpace` preallocates once, reused via `alive` — no per-shot `realloc` |
| 8. Float-aware collision | ⚠️ Partial | Still compares `(int)`-truncated positions for exact equality rather than a distance/epsilon check — the exact trap this exercise calls out. See `03_KNOWN_ISSUES.md`, item B2 |
| 9. Full loop structure | ✅ Done | `gamePlaying()`: input → update → resolve collisions → clamp → render, every frame |

Part 3 is, for practical purposes, done. The one loose thread (#8) is small and self-contained.

## Part 4 — audit

| Exercise | Status | Note |
|---|---|---|
| 1. Game state machine | 🟡 Started | Enum + `switch` + handler stubs exist. `MAIN_MENU` forces `gamePlaying()` instead of using `gameMenu()`; `PAUSED`/`GAME_WIN`/`GAME_LOSE` are unreachable and empty; `gameMenu()` itself has a real bug (`03_KNOWN_ISSUES.md`, item A3) |
| 2. Fixed timestep + accumulator | ⬜ Not started | Flat assumed 16ms/frame + `sleep`; no measured elapsed time feeding an accumulator |
| 3. Input → semantic actions | ⬜ Not started | Gameplay still checks `KEY_A`/`KEY_D`/etc. directly; no `Action` enum or key→action mapping layer |
| 4. Wave / level progression | ⬜ Not started | 5 enemies, placed once, no respawn/scaling/win condition |
| 5. Component-based structure (ECS-lite) | 🟡 Started | `Vec2`/`Body` exist and work; `units.h/.c` is a half-done extraction of the old structs. Neither is wired into gameplay |
| 6. Memory audit (Valgrind) | ⬜ Not run | Two real issues already found by inspection, before even running Valgrind — see `03_KNOWN_ISSUES.md`, section A |
| 7. Configuration system | ⬜ Not started | Map size, speeds, cooldowns, `MAX_BULLETS` all hardcoded |
| 8. Event system | ⬜ Not started | Collision code calls `removeEnemy` directly; no event queue/dispatch |

## A suggested order for Part 4

Not because the original 1–8 numbering is wrong — just that a few things you've already half-started make natural on-ramps, and a couple of exercises quietly want others to be solid first.

1. **Finish the `units`/component split (Exercise 5, first half).** You're already partway in. It's mostly mechanical: move the struct definitions properly into `units.h`/`units.c`, add the missing include guard, remove the duplicate copies from `main.c`. Doing this first means everything after this point gets written against the real headers instead of a duplicated set.
2. **Wire in `Vec2`/`Body` (Exercise 5, second half).** Once `Unit`/`Bullet` live in `units.h`, this is the natural moment to consider giving them a `Body` member instead of raw `x, y, vx, vy` — you're already touching those structs.
3. **A quick memory pass (part of Exercise 6).** While you're restructuring anyway, this is a cheap time to fix the two concrete bugs already found (`03_KNOWN_ISSUES.md`, items A1 and A2) rather than let them ride. A full Valgrind pass can come later once there's more surface area worth auditing.
4. **Fixed timestep + accumulator (Exercise 2).** Do this before adding more gameplay systems — waves and events should be built on correct timing from the start, not reworked later to fit it.
5. **Finish the state machine (Exercise 1, rest of it).** A real menu, a working pause, and an actual path into `GAME_WIN`/`GAME_LOSE`. Fix the `gameMenu()` key-indexing bug here, since you'll be re-enabling that call.
6. **Input → action abstraction (Exercise 3).** Natural to pair with #5 — "what a key means" often depends on which state you're in.
7. **Waves (Exercise 4).** This is where theme has the most room to live — see `04_NECROMANCER_GAME_DESIGN.md`. Wants the state machine and timestep solid first.
8. **Event system (Exercise 8).** Pairs naturally with waves and combat (`ENEMY_KILLED`, `WAVE_COMPLETE`, maybe `UNIT_RAISED`).
9. **Config system (Exercise 7).** Lower risk, genuinely fine to do whenever you want a break from architecture work.

Part 4's own document has a good self-check at the end — the outcome checklist and the six review questions. Worth re-reading that once you think you're close to done with this Part, rather than reproducing it here.

## The long view: Part 5

Part 4's doc previews Part 5 as an SDL2 port — and it's explicit that this is meant to be a port, not a rewrite: the loop, the state machine, the component structure, the event queue all carry over unchanged. SDL is framed as strictly an I/O upgrade (real input, real rendering, cross-platform windowing/audio/timing), not a replacement for any of the architecture work you're about to do in Part 4. Worth keeping in mind so Part 4 doesn't feel like throwaway work — it isn't.
