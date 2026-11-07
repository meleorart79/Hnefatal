
#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include "pieces.h"

#define BOARD_SIZE (ROW_COUNT * COLUMN_COUNT)
#define POSITION_STRING_SIZE 132

typedef enum {
    COLUMN_A, COLUMN_B, COLUMN_C, COLUMN_D, COLUMN_E, COLUMN_F,
    COLUMN_G, COLUMN_H, COLUMN_I, COLUMN_J, COLUMN_K, COLUMN_COUNT
} Column;

typedef enum {
    ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6,
    ROW_7, ROW_8, ROW_9, ROW_10, ROW_11, ROW_COUNT
} Row;

typedef struct {
    char column;
    int row;
} Position;

typedef enum {
    Edge, Castle, Open
} CType;

typedef struct {
    Column column;
    Row row;
    CType type;
    Piece piece;
    bool occupied;
} Cell;

typedef struct {
    Cell cells[BOARD_SIZE];
} Board;

typedef struct {
    Board board;
} GameState;

const char* game_title(void);
const char* game_version(void);

Board board_make(void);
void board_setup(Board* board);
bool board_position_from_string(Board* board, const char* position);
bool board_position_to_string(const Board* board, char position[POSITION_STRING_SIZE]);

void game_init(GameState* game);

#endif

