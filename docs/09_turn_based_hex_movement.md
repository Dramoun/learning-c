# Turn-Based Hex Movement

*A migration plan, not a spec — this documents the decision to drop continuous `Body`/`Vec2` movement in favor of discrete, one-tile-at-a-time hex movement, and lays out what actually has to change in the code to get there. Read alongside `01_PROJECT_STATUS.md` (current state) and `05_PROJECT_N_CONCEPT.md` (where this is ultimately headed — a commander directing units reads a lot better on tiles than on floats).*

## The decision

Positions stop being `Vec2 pos` updated by velocity/acceleration each frame, and become integer hex coordinates updated one step at a time, on a turn. No more `bodyUpdate`, no more accumulator-driven physics for movement, no more "how close is close enough" distance checks — a unit is either standing on a tile or it isn't, and two units either share a tile or they don't.

Animation (units sliding smoothly between tiles instead of popping) is explicitly **out of scope here** — that's a rendering-layer concern to solve later, probably by lerping a separate visual position toward the logical hex each frame. This doc is only about the source of truth for *where a unit actually is* and *when it's allowed to move*.

## What gets deleted or stops being used for movement

- **`bodyUpdate` / `bodyUpdateWithAcceleration` / `bodyApplyForce`** (`math/body.c`) — this is Euler integration of velocity and acceleration. Turn-based, tile-to-tile movement has no velocity or acceleration to integrate. This code doesn't need to be deleted from the repo necessarily, but nothing in the unit-movement path should call it anymore.
- **`Body.vel` and `Body.acc`** — dead weight for units once movement is tile-based. `Body.pos` also stops being the logical position.
- **The `dt`-scaled accumulator loop driving movement in `main.c`** — right now `bodyUpdate(&game->player->body, dt)` runs up to `maxUpdates` times per frame inside the fixed-timestep accumulator (see `06.5_FIXED_TIMESTEP_IMPLEMENTATION.md`). That accumulator can stay for anything genuinely time-based later (status effect durations, animation timers), but it should no longer be what advances a unit's position.
- **`playerMoveLeft/Right/Up/Down`** (`units.c`) as currently written — they're presumably nudging `body.acc` or `body.vel` continuously while a key is held. That whole "held key = continuous movement" model goes away. A move is a single discrete action: press a direction, unit steps one hex, done — not "held key = drift in that direction."
- **`twoBodyCollisionCheck`** (radius + distance-squared) — replaced by a straight equality check: are two units' `Hex` coordinates the same. No radius, no hitbox float.
- **`isPointInsideRoom`'s** current `Vec2 p` signature — bounds-checking becomes "is this `Hex` in the room's tile set," not "is this float point inside this polygon." The polygon (`RoomShape.vertices`) can still be the *authoring* format (draw a room shape, then bake it into a set of hexes once), but runtime checks shouldn't be doing float-in-polygon math every frame.

## What `Vec2` is still for

`Vec2` isn't going away as a type — `math/vec2.c` remains a perfectly good general-purpose 2D math module. It's just no longer what a `Unit`'s logical position is stored in. Realistic remaining uses:
- Converting a `Hex` to a pixel/screen position for rendering (`hexToPixel`), once there's real rendering.
- The eventual animation/lerp layer mentioned above.
- Anything genuinely continuous that isn't unit position (particle effects, camera easing, etc., down the road).

## The new data model

Axial coordinates, not cube — same math, one fewer redundant axis (`s = -q - r` if you ever need it, e.g. for hex-to-hex line drawing).

```c
// math/hex.h
typedef struct {
  i32 q;
  i32 r;
} Hex;

Hex hexAdd(Hex a, Hex b);
Hex hexSub(Hex a, Hex b);
bool hexEqual(Hex a, Hex b);
i32 hexDistance(Hex a, Hex b);           // (|q| + |r| + |q+r|) / 2
Hex hexNeighbor(Hex h, int direction);   // direction 0-5, fixed offset table
bool hexInRoom(Room *room, Hex h);       // replaces isPointInsideRoom for gameplay checks

Vec2 hexToPixel(Hex h, f32 size);        // rendering only
```

`Unit` (`units.h`) trades its reliance on `Body` for movement out for a `Hex tile` field:

```c
typedef struct {
  char symbol;
  Hex tile;
  Vec2 renderPos;   // optional, for the later animation layer — logical pos stays Hex
  u16 hp;
  bool alive;
} Unit;
```

Whether `Body` sticks around on `Unit` at all is your call — if nothing but movement was using it, it's fine to drop it from `Unit` entirely and keep `body.c`/`body.h` around unused for now, or repurpose it later for something that's actually continuous (real projectile arcs, knockback, etc.).

## Turn structure

The other half of "turn-based" is *when* a unit is allowed to move at all — this needs at least a minimal answer before movement code can be written:

- Simplest possible version: **strict alternating turns** — player acts (one move or one action), then all enemies act, repeat. This is enough to prove the loop and matches the current single-player-plus-enemies structure without inventing an initiative/speed system yet.
- `GameState` (`structures.h`) is a natural place to track whose turn it is — either a new field on `Game` (e.g. `TurnOwner turn`) or a dedicated state distinct from the existing `PLAYING` if turn phases need their own transitions (`PLAYER_TURN`, `ENEMY_TURN`).
- Input handling in `gamePlaying` (`main.c`) changes from "is this key currently down" to "was this key just pressed, and it's the player's turn" — a single move per keypress, not continuous movement while held. Your `KeyState` system (`terminal_utils`) already seems to distinguish `pressed` from `down`, so this is mostly about which one movement checks.
- Initiative/speed-based turn order, action points, multiple actions per turn — all reasonable later additions, deliberately left out of this doc's scope. Get one unit stepping one tile on its turn working first.

## Bullets

`Bullets`/`addBullet` (`units.c`) currently model bullets as free-flying `Body`-based units with their own velocity. That doesn't map cleanly onto a tile grid. Two honest options, pick one before touching this code:

1. **Instant resolution** — a ranged attack checks line-of-sight/range across hexes and resolves hit/miss immediately, no traveling projectile object at all. Simplest, and standard for turn-based tactics games.
2. **Cosmetic-only projectile** — keep a lightweight visual bullet that flies across the screen for feedback, but it's decorative; the hit is already decided the instant the action was taken. More visual polish, more code to keep in sync with a decision that's already made.

Leaning toward (1) for now — it's the smaller change and matches "lose the velocity crap" in spirit. (2) is a fine later upgrade once there's real rendering to make it worth the trouble.

## Suggested order of work

1. Add `math/hex.h` / `math/hex.c` — pure math, no dependencies on the rest of the game, easy to unit test in isolation (see `tests/test.c`).
2. Add `Hex tile` to `Unit`, stop reading `body.pos` for gameplay logic (rendering/`placeUnit` in `main.c` can keep using it a little longer if you want to defer the render-layer change).
3. Replace `playerMoveLeft/Right/Up/Down` with direction-based, one-step movement (`playerMove(Player *player, int direction)` calling `hexNeighbor`), gated on whose turn it is.
4. Replace `twoBodyCollisionCheck` usage for unit-vs-unit with `hexEqual`.
5. Decide and implement the bullets approach above.
6. Only once all of that works: revisit `main.c`'s accumulator loop — it can likely simplify a lot once nothing per-frame depends on it for movement.

## Open decisions worth making yourself

- **Strict alternating turns vs. any form of initiative** — flagged above, alternating is the cheaper starting point.
- **Does `Body` stay on `Unit` at all**, unused, in case something later genuinely needs continuous motion — or does it come off `Unit` cleanly now and get re-added if/when something needs it.
- **Room representation** — keep `RoomShape` polygons as the authoring format and bake to a hex set once, or author rooms as hex sets directly. Baking once is likely less disruptive to what already exists.
- **Bullets: instant resolution vs. cosmetic projectile** — leaning instant, but it's your call and affects how much of `units.c` gets touched.

None of this needs to be settled before starting — `hex.h`/`hex.c` (step 1) is safe to build regardless of how the later decisions land.
