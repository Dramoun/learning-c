#!/usr/bin/env bash
set -e  # stop immediately if anything fails

# =========================
# TODO: BUILD SYSTEM NOTES (READ THIS)
# =========================
#
# This script is your CURRENT simple build system.
# It is meant for learning, not for production.
#
# You will eventually replace it with a Makefile.
#
# -------------------------
# CURRENT STATE
# -------------------------
#
# Right now:
# - Uses clang directly
# - Compiles all .c files at once
# - Uses C99 standard for maximum compatibility
# - No dependency tracking (everything rebuilds every time)
#
# This is OK for learning.
#
# -------------------------
# C STANDARD (IMPORTANT)
# -------------------------
#
# You are currently using:
#     -std=c99
#
# That means:
# - Very stable and widely supported
# - Missing modern syntax/features
#
# Future upgrade:
#
#     -std=c23
#
# What C23 gives you:
# - cleaner standard library additions
# - better type utilities (like stdbit.h)
# - more modern syntax improvements
#
# IMPORTANT TRUTH:
# - C23 does NOT make code faster by itself
# - Speed comes from compiler optimization (-O2, -O3)
# - and CPU features (-march=native)
#
# So upgrade C version for convenience, NOT performance.
#
# Recommended future build flags:
#
#     clang -std=c23 -O2 -march=native
#
# -------------------------
# FUTURE PROJECT STRUCTURE (WITH SDL2)
# -------------------------
#
# Example folder layout:
#
# project/
#   src/
#     main.c
#     engine/
#       input.c
#       render.c
#     game/
#       player.c
#
#   lib/
#     SDL/        (git clone of SDL2 here)
#
#   build/
#
# -------------------------
# SDL2 USAGE (IMPORTANT IDEA)
# -------------------------
#
# If SDL2 is inside:
#     lib/SDL
#
# You will compile like this:
#
# clang -std=c23 -O2 -Ilib/SDL/include \
#       -Isrc \
#       src/*.c \
#       -Llib/SDL/build \
#       -lSDL2 \
#       -o build/main
#
# EXPLANATION:
# - -I = where headers are (SDL.h)
# - -L = where compiled SDL library is
# - -lSDL2 = link SDL2 binary
#
# -------------------------
# WHY YOU WILL SWITCH TO MAKEFILE
# -------------------------
#
# This script is simple, but not scalable.
#
# Problems:
# - recompiles everything every time
# - no dependency tracking
# - manual file handling (*/*.c)
#
# Makefile solves this.
#
# -------------------------
# SIMPLE MAKEFILE EXAMPLE (STARTER LEVEL)
# -------------------------
#
# This is what your future Makefile might look like:
#
# CC = clang
# CFLAGS = -std=c23 -O2 -Ilib/SDL/include -Isrc
# LDFLAGS = -Llib/SDL/build -lSDL2
#
# SRC = $(wildcard src/*.c src/*/*.c)
# OUT = build/main
#
# all:
# 	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)
#
# run:
# 	./build/main
#
# clean:
# 	rm -rf build/*
#
# -------------------------
# WHAT THIS TEACHES YOU
# -------------------------
#
# - how C compilation actually works
# - how libraries (like SDL2) are linked
# - how project structure affects builds
# - how build systems scale with complexity
#
# -------------------------
# RULE OF LEARNING
# -------------------------
#
# Step 1: simple script (you are here)
# Step 2: Makefile (next step)
# Step 3: advanced build systems (CMake, etc.)
#
# Do NOT skip steps.
# Complexity without understanding = confusion later.
#
# =========================

SRC_DIR="src"        # real engine / game code
TEST_DIR="tests"     # sandbox / experiments
BUILD_DIR="build"    # compiled binaries go here

MAIN_OUT="$BUILD_DIR/main"
TEST_OUT="$BUILD_DIR/test"

# create build folder if it doesn't exist
mkdir -p "$BUILD_DIR"

# =========================
# MODE SWITCH
# =========================
# usage:
#   ./build.sh        -> build + run main game
#   ./build.sh test   -> build + run test sandbox

MODE="${1:-main}"

# =========================
# TEST MODE
# =========================
if [ "$MODE" = "test" ]; then
    echo "Building TEST program..."

    clang -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=199309L \
        "$TEST_DIR/test.c" \
        -o "$TEST_OUT"

    echo "Running TEST..."
    "$TEST_OUT"
    exit 0
fi

# =========================
# MAIN BUILD (ENGINE)
# =========================
echo "Building MAIN program..."

clang -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=199309L \
    -I"$SRC_DIR" \
    "$SRC_DIR"/*.c "$SRC_DIR"/*/*.c \
    -o "$MAIN_OUT"

echo "Running MAIN..."
"$MAIN_OUT"
