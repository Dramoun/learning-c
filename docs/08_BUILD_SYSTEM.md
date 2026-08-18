# Build System

*How this project is built, why it's set up this way, and how to keep it working as you add to it. Written and verified against a real Linux Mint 22.x-equivalent environment (Ubuntu 24.04 base) on 2026-08-11.*

## TL;DR

```bash
# Fast path — terminal prototype, no SDL3, no extra installs
make run

# Full path — terminal prototype + SDL3, via CMake
git submodule update --init --recursive   # once
cmake --preset default
cmake --build --preset default
./build/main
```

If either fails, jump to [Troubleshooting](#troubleshooting).

---

## Why two build systems

You'll find a `Makefile` *and* a `CMakeLists.txt` in the repo root. That's intentional, not leftover clutter — they serve different moments:

- **`Makefile`** compiles only `src/`, with no external dependencies. It's for when you're heads-down on gameplay/systems code (the state machine, collision, the `units`/`vec2`/`body` refactor) and just want the fastest possible edit → compile → run loop.
- **`CMakeLists.txt`** additionally builds and links **SDL3** (vendored as a git submodule at `lib/SDL`). It's for once you start the SDL port (`06_PATH_TO_PROJECT_N.md`, Stage 2). CMake is what makes "compile a ~380-file C library and link it correctly" a non-event instead of a weekend.

Today, `main.c` doesn't call any SDL function yet — the terminal input in `src/utils/terminal_utils.c` is explicitly a stand-in until then (its own doc comment says so). So the Makefile is genuinely the right default *right now*, not a fallback. Switch to CMake when the code does.

Both build with the same warnings and the same C standard, so nothing behaves differently between them.

---

## Prerequisites (Linux Mint / Ubuntu / Debian)

**Fast path only:**

```bash
sudo apt update
sudo apt install build-essential clang git
```

`clang` isn't strictly required (the Makefile also works with `gcc`, see [The C standard flag](#the-c-standard-flag-c23-vs-c2x) below) but it's the default and the one that gets you `-std=c23` directly with nothing extra to configure.

**Full path** — everything above, plus CMake, Ninja, and SDL3's own Linux build dependencies:

```bash
sudo apt update
sudo apt install \
  build-essential git pkg-config cmake ninja-build clang \
  libasound2-dev libpulse-dev libaudio-dev libfribidi-dev libjack-dev libsndio-dev \
  libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev \
  libxi-dev libxss-dev libxtst-dev libxkbcommon-dev \
  libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev \
  libdbus-1-dev libibus-1.0-dev libudev-dev libthai-dev libusb-1.0-0-dev \
  libpipewire-0.3-dev libwayland-dev libdecor-0-dev liburing-dev
```

This is SDL3's own documented Ubuntu 22.04+ dependency list (from `wiki.libsdl.org/SDL3/README-linux`), which is what Mint 22.x matches — trimmed only of the packages needed for SDL's *own* test suite, which this project doesn't build (`SDL_TEST_LIBRARY` is off, see the CMakeLists). If you ever see a CMake configure line like `SDL_JACK (Wanted: ON): OFF` and want that backend, it just means one of these packages is missing — install it and reconfigure.

This was tested end-to-end in a fresh Ubuntu 24.04 container on 2026-08-11: full SDL3 static build + game, submodule fetch to running binary, in under two minutes.

---

## Full path, step by step

```bash
git clone https://github.com/Dramoun/learning-c.git
cd learning-c
git submodule update --init --recursive   # fetches SDL3 into lib/SDL
cmake --preset default                    # configure (first time, or after editing CMakeLists.txt)
cmake --build --preset default            # build (every time after that)
./build/main
```

**The submodule step is easy to miss and the failure looks confusing if you do.** `lib/SDL` is a *git submodule* (see `.gitmodules`), not a normal tracked folder — a plain `git clone` leaves it as an empty directory. If you try to build without initializing it, CMake now stops with an explicit message telling you to run the command above, rather than failing deep inside SDL's own CMake with a confusing error.

**Day to day**, once it's set up:

```bash
cmake --build --preset default
```

is the only command you need — thanks to `CONFIGURE_DEPENDS` (see below), even adding a new `.c` file doesn't require re-running the configure step.

**Optimized build**, when you want to see real performance instead of a debug build:

```bash
cmake --build --preset release
./build-release/main
```

### What the presets are

`CMakePresets.json` exists so nobody has to remember generator flags or where the build directory is. `cmake --preset default` is shorthand for `cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug`. Two presets are defined: `default` (debug, `build/`) and `release` (optimized, `build-release/`). If you want your own personal variant (a sanitizer build, say), create a `CMakeUserPresets.json` — it's already gitignored, so it stays local to you.

---

## The C standard flag: C23 vs C2x

Both build files target the newest C standard your compiler supports, but *which flag* means that depends on the compiler:

| Compiler (from apt, Mint 22.x) | Version | Flag it needs |
|---|---|---|
| `clang` | 18.x | `-std=c23` |
| `gcc` | 13.x | `-std=c2x` |

C23 (ISO/IEC 9899:2024) is the current published C standard — there's nothing newer to target yet; the next revision (informally "C2y") isn't due until 2029. `c2x` was C23's working-draft name before it was finalized, and GCC kept accepting it as a flag rather than renaming it; GCC only started honoring the literal `-std=c23` spelling in GCC 14, and only made C23 its *default* in GCC 15. Mint 22.x's stock `gcc` (13.3, tied to Ubuntu 24.04) predates that, so `-std=c23` on stock `gcc` fails outright with *"unrecognized command-line option"* — it's not that the standard is unsupported, just that flag spelling is. `clang` from the same `apt install` is already version 18, which accepts `-std=c23` directly. This is confirmed by actually building both ways in the environment this doc was written in, not just documentation.

Both build files handle this for you automatically instead of hardcoding one:

- **Makefile:** `STD_FLAG` runs a one-line compiler probe (`$(CC) -std=c23 -E ...`) and falls back to `-std=c2x` if that fails.
- **CMakeLists.txt:** the `CheckCCompilerFlag` module does the same probe.

So the project always builds with the best the current compiler offers — including automatically switching to the plain `-std=c23` spelling the moment you upgrade past GCC 14, with no file edit needed. `make CC=gcc` (or editing `CC` in the Makefile) forces GCC if you want it for some reason (e.g. comparing warnings between compilers).

If you want the newest standard features as they land (rather than just the newest *stable* one), see [Updating your toolchain](#updating-your-toolchain).

---

## Adding a new source file

**Both build systems auto-detect new `.c` files** — there's no file list to hand-maintain.

1. Drop your file under `src/` (in a subfolder like `src/components/`, or a new one — both work).
2. Give any new header an include guard (`#ifndef`/`#define`/`#endif`) — `units.h` is currently the one exception; add the guard if you're the one wiring it in.
3. Build as usual:
   - Makefile: `make` — `$(wildcard src/*.c src/*/*.c)` re-evaluates every run.
   - CMake: `cmake --build --preset default` — `file(GLOB_RECURSE ... CONFIGURE_DEPENDS ...)` re-checks the file list and re-triggers configure automatically if it changed. (`CONFIGURE_DEPENDS` is a well-supported CMake/Ninja/Makefiles extension; if you ever switch to a generator that doesn't honor it, `cmake -S . -B build` re-syncs the list manually.)

A new *nested* subfolder (`src/foo/bar/baz.c`) is picked up the same way by both — the Makefile's `src/*/*.c` only goes one level deep, so a two-levels-deep new file needs `src/*/*/*.c` added if you ever go that far; CMake's recursive glob has no such limit.

---

## Adding a new third-party library

The project already does this once, for SDL3 — new libraries can follow the same pattern:

1. **Vendor it as a submodule:**
   ```bash
   git submodule add https://github.com/<org>/<repo>.git lib/<name>
   ```
2. **Wire it into `CMakeLists.txt`**, same shape as the SDL3 block:
   ```cmake
   add_subdirectory(lib/<name> EXCLUDE_FROM_ALL)
   target_link_libraries(main PRIVATE <name>::<name>)   # check the library's own docs for its exact target name
   ```
3. Commit both the `.gitmodules` change and the `CMakeLists.txt` change together.

**Alternative: `FetchContent`.** For a library you don't need to pin-and-edit locally, CMake can fetch it during configure instead of needing a separate `git submodule` step at all:

```cmake
include(FetchContent)
FetchContent_Declare(
  somelib
  GIT_REPOSITORY https://github.com/<org>/<repo>.git
  GIT_TAG        v1.2.3   # pin a real tag, not a moving branch
)
FetchContent_MakeAvailable(somelib)
target_link_libraries(main PRIVATE somelib::somelib)
```

Submodule vs. `FetchContent` is mostly a workflow preference — submodules show up as an explicit path in the repo and are easy to inspect/patch locally; `FetchContent` needs one less manual step per clone. Either is a completely normal, current choice; SDL's own official CMake docs show the submodule pattern as the standard way to vendor SDL itself, which is why that's what's already here.

---

## Updating SDL3

The submodule currently points at a specific SDL3 commit (pinned back on 2026-04-08). To move it forward:

```bash
cd lib/SDL
git fetch origin
git checkout main          # or a specific release tag, e.g. git checkout release-3.4.14
cd ../..
git add lib/SDL
git commit -m "Update SDL3"
cmake --preset default     # reconfigure - re-links against the new version
cmake --build --preset default
```

Pinning to a release tag instead of tracking `main` trades "always latest" for "won't break under you mid-project" — reasonable to switch to once the SDL port is underway and stable matters more than bleeding-edge.

---

## Updating your toolchain

Stock Mint 22.x is intentionally conservative (that's the point of an LTS base), so "latest" sometimes means going slightly outside `apt`:

- **Newer Clang:** `apt install clang` already gets you Clang 18 (full `-std=c23`), so there's no urgency here. If you want a bleeding-edge Clang anyway (e.g. to try C2y preview features), LLVM publishes an official install script for Debian/Ubuntu at `apt.llvm.org` — that's the standard, maintainer-blessed way to get a newer version than your distro ships.
- **Newer GCC:** the `ubuntu-toolchain-r/test` PPA is the usual community source for GCC versions newer than an LTS release's default; check what it currently offers for Ubuntu 24.04 (`noble`) before adding it, since PPA contents shift over time. This project's compiler-detection logic (above) means you don't *need* to do this for anything current to work — it's only relevant if you specifically want to experiment with GCC's take on newer-than-C23 features.

Neither is required for this project to build or to use C23 — both build files already get you there with the compilers `apt install` gives you today.

---

## Editor / LSP setup (clangd)

`CMAKE_EXPORT_COMPILE_COMMANDS ON` (already set) generates `build/compile_commands.json` on every CMake configure — that's the file `clangd` (used by most editors' LSP integration, including VS Code's clangd extension, Neovim, etc.) needs to understand your `#include`s and give real autocomplete/diagnostics. After your first CMake build:

```bash
ln -s build/compile_commands.json .
```

You only need to redo anything if you add a new library or change include paths — day-to-day source edits don't require touching this again. (The Makefile path doesn't produce this file; if you want clangd support while only using the fast path, an easy option is running the CMake configure step once — `cmake --preset default` — just to generate `compile_commands.json`, without necessarily using CMake for your actual day-to-day builds.)

---

## Troubleshooting

**`lib/SDL is empty, so SDL3 can't be built`** (CMake configure error)
You cloned without submodules. Run `git submodule update --init --recursive`.

**`gcc: error: unrecognized command-line option '-std=c23'`**
You're on stock Mint/Ubuntu `gcc` (13.x) and something bypassed the flag-detection logic — e.g. you ran the compiler manually instead of through `make`/`cmake --build`. Use `-std=c2x` directly, or just build through the Makefile/CMake as documented above, which picks the right flag for you.

**CMake configure shows a backend as `OFF` that you expected `ON`** (e.g. `SDL_PULSEAUDIO (Wanted: ON): OFF`)
A `-dev` package for that backend is missing. Cross-check against the [Prerequisites](#prerequisites-linux-mint--ubuntu--debian) list, install the missing one, and re-run `cmake --preset default`.

**First CMake build feels slow**
That's SDL3 compiling from source (~380 files) — expect roughly a minute or two on a normal desktop, once. Every build after that only recompiles what changed, both for SDL and for `src/`.

**`make` picks `cc`/`gcc` when you expected `clang`**
If you edited the Makefile and changed `CC = clang` to `CC ?= clang`, that's why: GNU Make's *built-in* default (`CC` = `cc`) has an internal origin of `default`, and `?=` only assigns when a variable is completely undefined — so it silently loses to Make's own built-in before your line ever runs. Plain `CC = clang` (as shipped) overrides that built-in default correctly, while `make CC=gcc` on the command line still overrides *that*, since command-line assignments always win. Worth knowing if you ever restructure the Makefile.

**Terminal input feels unresponsive / arrow keys don't register as expected**
That's a real, already-known limitation of the current hand-rolled terminal input (see `03_KNOWN_ISSUES.md`, B1) — not a build problem.

---

## Graphics library: staying on SDL3

You'd already scaffolded SDL3 (submodule + CMake target) before asking about this, and it's worth confirming that's still the right call rather than quietly assuming it:

- **SDL3** hit its stable 3.0 release in January 2025 and is actively developed (3.4.x as of mid-2026) — mature, not a bleeding-edge gamble. It's low-level: windowing, input, audio, a basic 2D/3D render API, nothing game-shaped on top. For a project explicitly about learning C and systems programming, that's a feature — you'll build your own render/asset/entity layer on top of real primitives, which is exactly the kind of thing `05_PROJECT_N_CONCEPT.md` and the roadmap docs are already pointed at.
- **raylib** is the other current, well-maintained option (v6.0, April 2026) worth knowing about even if you don't switch: much higher-level (`DrawRectangle()`, built-in asset loading, an immediate-mode GUI module), outstanding docs/examples, and also plain C — so it wouldn't compromise the "learning C" goal either. It trades away some of the "understand what's actually happening" value SDL3 gives you, in exchange for getting pixels on screen faster.

Given the amount of design work already aimed at "the SDL port" specifically (roadmap Part 5, Project N docs), staying on SDL3 is the coherent choice — this section exists so that's a confirmed decision with current information behind it, not an assumption. If at some point the priority shifts from "understand the systems" to "ship the game faster," raylib is the one other option worth reaching for, not a long list.

---

## What changed in this pass (2026-08-11)

For transparency — this is what was actually broken and what changed, based on cloning the repo fresh and testing both build paths end-to-end:

- **`src/main.c`**: fixed a compile-breaking typo in `gameMenu()` — `key[KEY_P]` → `keys[KEY_P]`. This is the same spot `03_KNOWN_ISSUES.md` item A3 already flagged (a different bug there, `keys['p']` out-of-bounds indexing), which had been half-fixed by hand since that doc was written, leaving this typo in its place. The project would not compile at all before this fix, under either build system.
- **`CMakeLists.txt`**: rewritten. Previously: `file(GLOB SRC_FILES src/*.c)` only matched `src/main.c` and silently missed every file in `src/components/`, `src/math/`, `src/utils/` (non-recursive glob), which would have failed at the link step with undefined-reference errors the moment SDL code got wired in; `target_include_directories` pointed at a non-existent `include/` folder; the C standard was pinned to C99 with no path to newer; there was no guard for an uninitialized submodule. All fixed; SDL3 now also builds static by default (single self-contained binary) with its test-library build off.
- **`Makefile`**: added compiler-aware `-std=c23`/`-std=c2x` detection (previously hardcoded `-std=c23`, which silently doesn't exist on stock Mint/Ubuntu `gcc`, only `clang`).
- **`CMakePresets.json`**: new — one-command configure/build instead of remembering generator/build-dir flags.
- **`.gitignore`**: added `build-release/` and `CMakeUserPresets.json`.
- **`CMAKEREADME.md`**: removed. It described an older manual `vendored/SDL` clone workflow that no longer matches the actual submodule-based setup in `.gitmodules`/`CMakeLists.txt`; its accurate content is folded into this doc.
- **`README.md`**: rewritten as a short entry point that links here instead of trying to hold everything itself.

Both build paths were tested to a fully working, running binary (Makefile with both `clang` and `gcc`; CMake configure + full SDL3 static build + run) before writing any of this down.
