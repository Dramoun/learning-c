CC = clang

CFLAGS = -Wall -Wextra -std=c23 -D_POSIX_C_SOURCE=199309L -Isrc -g

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
