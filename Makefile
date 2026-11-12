CC      := gcc
CFLAGS  := -Isrc -std=c11 -Wall -Wextra -Wpedantic

SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=build/%.o)

TEST_LIB := src/ui.c src/board.c src/rules.c

hnefatal.exe: $(OBJ)
	$(CC) $(OBJ) -o $@

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

build:
	mkdir -p build

run: hnefatal.exe
	./hnefatal.exe

build/test_board.exe: tests/test_board.c $(TEST_LIB) | build
	$(CC) $(CFLAGS) $^ -o $@

build/test_rules.exe: tests/test_rules.c $(TEST_LIB) | build
	$(CC) $(CFLAGS) $^ -o $@

build/test_capture.exe: tests/test_capture.c $(TEST_LIB) | build
	$(CC) $(CFLAGS) $^ -o $@

test: build/test_board.exe build/test_rules.exe build/test_capture.exe
	./build/test_board.exe
	./build/test_rules.exe
	./build/test_capture.exe

clean:
	rm -rf build hnefatal.exe

-include $(OBJ:.o=.d)

.PHONY: run clean test