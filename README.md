# learning-c

A real-time, terminal-rendered 2D action game written in C — a hands-on vehicle for learning modern C and systems programming, with a longer-term destination as an SDL3-rendered "commander necromancer" roguelike. See `01_PROJECT_STATUS.md` for where the code currently stands and `05_PROJECT_N_CONCEPT.md` for where it's headed.

## Quick start (Linux Mint / Ubuntu / Debian)

There are two build paths. Use the fast one day-to-day; switch to the full one once you're working on the SDL3/graphics port.

**Fast path** — no dependencies beyond a compiler, builds the terminal prototype:

```bash
make run
```

**Full path** — builds the same code plus SDL3, via CMake:

```bash
git submodule update --init --recursive   # first time only - fetches SDL3
cmake --preset default
cmake --build --preset default
./build/main
```

First CMake build compiles SDL3 from source, so it takes a couple of minutes. Everything after that is incremental and fast.

Full prerequisites, what each command does, how to add a source file or a new library, how to update SDL3, and troubleshooting all live in **`08_BUILD_SYSTEM.md`** — read that before touching the build system.

## Repo layout

```
src/                   # game code
  main.c                # game loop, state machine, current gameplay (still monolithic)
  math/                 # Vec2 / Body - ready, not yet wired into gameplay
  components/           # in-progress extraction of Unit/Bullet/etc. out of main.c
  utils/                 # terminal input, standing in for SDL3 input until the port
tests/                 # standalone experiments (not part of the main build)
lib/SDL/               # SDL3, as a git submodule - see 08_BUILD_SYSTEM.md
CMakeLists.txt         # full build (SDL3)
CMakePresets.json      # `cmake --preset default`
Makefile               # fast build (no SDL3)
```

## Docs

| Doc | What it's for |
|---|---|
| `01_PROJECT_STATUS.md` | Snapshot of what's built and working right now |
| `02_ROADMAP.md` | Exercise-by-exercise progress against the learning plan |
| `03_KNOWN_ISSUES.md` | Bugs and rough edges found during review (mostly still accurate — see note below) |
| `04_NECROMANCER_GAME_DESIGN.md` | Early design brainstorm for the necromancer theme |
| `05_PROJECT_N_CONCEPT.md` | The fuller long-arc game concept |
| `06_PATH_TO_PROJECT_N.md` / `06.5_...md` | Staged plan to get from here to there; fixed-timestep implementation notes |
| `07_FUTURE_PLANS_AND_IDEAS.md` | Later-stage design ideas |
| `08_BUILD_SYSTEM.md` | **Build system reference** - start here for anything about building, adding files, adding libraries, or updating toolchain/dependencies |

> **Note:** while fixing the build, `03_KNOWN_ISSUES.md` item A3 (`gameMenu()`'s key check) turned out to already be half-fixed by hand, with a new one-character typo (`key` → should be `keys`) left in its place, which was breaking compilation outright. Fixed in `src/main.c` — see `08_BUILD_SYSTEM.md` for the full "what changed" list. The rest of that doc still reflects the current code.
