## Setup (first time only)

Clone SDL manually:

```bash
git clone https://github.com/libsdl-org/SDL.git vendored/SDL
```

---

## Build project

```bash
cmake -S . -B build
cmake --build build
```

Run:

```bash
./build/main
```

---

## How this works

* `vendored/SDL` is treated as part of your project
* `add_subdirectory()` builds SDL automatically
* `target_link_libraries()` links it

You do NOT build SDL separately.

---

## Add new `.c` file

Put it in:

```id="srcdir"
src/
```

CMake auto-detects it (via `file(GLOB ...)`)

---

## Add new `.h` file

Put it in:

```id="incdir"
include/
```

Include like:

```c
#include "render.h"
```

---

## Rebuild after changes

```bash
cmake --build build
```

If CMakeLists changed:

```bash
cmake -S . -B build
```

---

## Clean build

```bash
rm -rf build
```

---

## clangd (LSP setup)

After first build:

```bash
ln -s build/compile_commands.json .
```

This file is generated because of:

```cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

---

## Important note about clangd

> “do I need to rebuild every time?”

No.

* Build once → generates `compile_commands.json`
* clangd uses that
* Only re-run CMake if:

  * you add libraries
  * change include paths

---

## Add another library (same style as SDL)

1. Clone into `vendored/`:

```bash
git clone <repo> vendored/libname
```

2. Add to CMake:

```cmake
add_subdirectory(vendored/libname)

target_link_libraries(main PRIVATE libname)
```

---

## Common mistakes

### Building SDL separately

Don’t do this:

```bash
cd vendored/SDL
cmake ..
```

Not needed.

CMake handles it via `add_subdirectory`.

---

### Missing compile_commands.json

clangd won’t work properly.

Fix:

```bash
ln -s build/compile_commands.json .
```

---

### Forgetting to re-run cmake

If you edit `CMakeLists.txt`:

```bash
cmake -S . -B build
```
