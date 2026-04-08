#!/usr/bin/env bash
set -e  # exit on any error

# Project variables
SRC_DIR="src"
BUILD_DIR="build"
OUTPUT="$BUILD_DIR/main"

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Compile all C files in src/
clang -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=199309L "$SRC_DIR"/*.c -o "$OUTPUT"

# Run the compiled program
"$OUTPUT"
