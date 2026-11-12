#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

#define BOARD_SIZE (ROW_COUNT * COLUMN_COUNT)
#define POSITION_STRING_SIZE 132
#define INIT_PIECE_COUNT 37

typedef enum {
    COLUMN_A, COLUMN_B, COLUMN_C, COLUMN_D, COLUMN_E, COLUMN_F,
    COLUMN_G, COLUMN_H, COLUMN_I, COLUMN_J, COLUMN_K, COLUMN_COUNT
} Column;

typedef enum {
    ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6,
    ROW_7, ROW_8, ROW_9, ROW_10, ROW_11, ROW_COUNT
} Row;

typedef struct {
    int row;
    char column;
} Position;

typedef enum {
    PARTY_D, PARTY_A,
    PARTY_COUNT, PARTY_NONE
} Party;

typedef enum {
    RANK_P, RANK_K,
	RANK_COUNT, RANK_NONE
} Rank;

typedef struct {
    Party party;
    Rank rank;
    int id;
} Piece;

typedef enum {
    Edge, Castle, Open
} CType;

typedef struct {
    Column column;
    Row row;
    CType type;
    Piece piece;
} Cell;

typedef struct {
    Cell cells[BOARD_SIZE];
} Board;

Board board_make(void);
bool board_setup(Board* board); 
bool board_position_from_string(Board* board, const char* position);
bool board_position_to_string(const Board* board, char position[POSITION_STRING_SIZE]);

bool board_init(Board* board);

#endif

