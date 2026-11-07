CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -g -Isrc -IC:/raylib/raylib/src
LDFLAGS := -LC:/raylib/raylib/src
LDLIBS  := -lraylib -lopengl32 -lgdi32 -lwinmm

SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=build/%.o)

TEST_LIB := src/pieces.c src/game.c src/rules.c

hnefatal.exe: $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS) $(LDLIBS)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

build:
	mkdir -p build

run: hnefatal.exe
	./hnefatal.exe

build/test_pieces.exe: tests/test_pieces.c $(TEST_LIB) | build
	$(CC) $(CFLAGS) $^ -o $@

build/test_rules.exe: tests/test_rules.c $(TEST_LIB) | build
	$(CC) $(CFLAGS) $^ -o $@

test: build/test_pieces.exe build/test_rules.exe
	./build/test_pieces.exe
	./build/test_rules.exe

clean:
	rm -rf build hnefatal.exe

-include $(OBJ:.o=.d)

.PHONY: run clean test