# Quick, dependency-free build for the terminal prototype (no SDL3, no
# CMake - just the compiler). Good for the fast edit/compile/run loop
# while you're working on gameplay code that doesn't touch graphics.
#
# Once you're building against SDL3, use CMake instead (see
# 08_BUILD_SYSTEM.md) - it builds SDL3 for you, this Makefile does not.

CC = clang

# Use -std=c23 if this compiler understands it, otherwise fall back to
# -std=c2x (older spelling of the same standard - see 08_BUILD_SYSTEM.md
# for why both exist). Works out of the box with clang from apt (Clang 18+,
# understands c23) and with gcc from apt on Mint 22.x (gcc 13, needs c2x).
STD_FLAG := $(shell $(CC) -std=c23 -E -x c /dev/null >/dev/null 2>&1 && echo -std=c23 || echo -std=c2x)

CFLAGS = -Wall -Wextra $(STD_FLAG) -D_POSIX_C_SOURCE=199309L -Isrc -g

SRC = $(wildcard src/*.c src/*/*.c)

BUILD_DIR = build
MAIN_OUT = $(BUILD_DIR)/main

.PHONY: all run clean

all: $(MAIN_OUT)

$(MAIN_OUT): $(SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC) -lm -o $(MAIN_OUT)

run: all
	./$(MAIN_OUT)

clean:
	rm -rf $(BUILD_DIR)
