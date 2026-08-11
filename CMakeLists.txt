cmake_minimum_required(VERSION 3.21)

project(learning-c LANGUAGES C)

# =========================
# C standard
# =========================
# Always build with the newest C standard this compiler understands.
# Recent Clang/GCC accept "-std=c23" directly. Compilers that only know
# C23 by its pre-standardization draft name accept "-std=c2x" instead
# (same language, older flag spelling) - this is exactly the case for
# the GCC that Ubuntu 24.04 / Linux Mint 22.x ships by default (GCC 13).
# We probe for the flag that actually works instead of hardcoding one,
# so the project keeps building as your toolchain gets newer.
include(CheckCCompilerFlag)
check_c_compiler_flag("-std=c23" COMPILER_SUPPORTS_C23_FLAG)
if(COMPILER_SUPPORTS_C23_FLAG)
  set(C_STD_FLAG "-std=c23")
else()
  set(C_STD_FLAG "-std=c2x")
endif()

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF) # strict ISO C; we opt into POSIX explicitly below

# Required for clangd / any LSP (also used by CMakePresets.json)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Put the built binary at build/main (not buried in a per-target folder)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# POSIX APIs (termios, nanosleep, clock_gettime, ...) used by the
# terminal input/timing code. Defined globally here so every source
# file gets it consistently, instead of per-file #defines.
add_compile_definitions(_POSIX_C_SOURCE=199309L)

# =========================
# SDL3 (vendored via git submodule)
# =========================
# lib/SDL is a git submodule (see .gitmodules), not a regular tracked
# directory. `git clone` alone leaves it empty - it must be fetched
# explicitly:
#
#   git submodule update --init --recursive
#
# 08_BUILD_SYSTEM.md covers this (and every other build step) in full.
set(SDL_SUBMODULE_CMAKE "${CMAKE_SOURCE_DIR}/lib/SDL/CMakeLists.txt")
if(NOT EXISTS "${SDL_SUBMODULE_CMAKE}")
  message(FATAL_ERROR
    "lib/SDL is empty, so SDL3 can't be built.\n"
    "Run this once, from the repo root, then re-run cmake:\n"
    "    git submodule update --init --recursive"
  )
endif()

# Build SDL3 as a single static library so the final binary is one
# self-contained file - no libSDL3.so to keep track of alongside it,
# no LD_LIBRARY_PATH/rpath issues when you move or share the binary.
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE) # we don't use SDL's test harness

add_subdirectory(lib/SDL EXCLUDE_FROM_ALL)

# =========================
# Your source files
# =========================
# GLOB_RECURSE + CONFIGURE_DEPENDS: picks up src/**/*.c automatically,
# including new subfolders, and re-runs cmake's configure step on the
# next build if the file list changed - so "add a .c file, build" is
# really all there is to it. (CONFIGURE_DEPENDS is a well-supported
# CMake/Ninja/Make extension; see 08_BUILD_SYSTEM.md if you ever hit a
# generator where it's not honored - a manual `cmake -S . -B build`
# always works as a fallback.)
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c")

add_executable(main ${SRC_FILES})

target_compile_options(main PRIVATE ${C_STD_FLAG} -Wall -Wextra)

# Your headers - everything currently lives under src/ (flat + subfolders)
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/src)

# =========================
# Link SDL3
# =========================
target_link_libraries(main PRIVATE SDL3::SDL3)
