/*
 * rules.h - drop-in replacement for your rules.h
 */
#ifndef RULES_H
#define RULES_H

#include <stdbool.h>
#include "board.h"

 /* Result of a move check. MOVE_OK means the move is legal;
  * every other value says which rule the move breaks.
  * The checks are done in this order, the first failure is returned. */
typedef enum {
    MOVE_OK = 0,
    MOVE_ERR_BAD_ARGUMENT,        /* a NULL pointer was passed                  */
    MOVE_ERR_OUT_OF_BOARD,        /* start or destination is outside A-K / 1-11 */
    MOVE_ERR_NO_PIECE,            /* the start square is empty                  */
    MOVE_ERR_NOT_YOUR_PIECE,      /* the piece belongs to the other player      */
    MOVE_ERR_SAME_SQUARE,         /* start and destination are the same square  */
    MOVE_ERR_NOT_STRAIGHT,        /* not along a row or a column                */
    MOVE_ERR_DESTINATION_OCCUPIED,/* another piece is on the destination        */
    MOVE_ERR_CASTLE_RESERVED,     /* only the king may stop on the castle       */
    MOVE_ERR_PATH_BLOCKED         /* a piece, or the castle, is in the way      */
} MoveResult;

/* Says why a move is refused (text shown to the player). */
const char* move_error_message(MoveResult result);

/* Checks a move for 'player'. Returns MOVE_OK or the first rule broken. */
MoveResult check_move(const Board* board, const Position* from,
    const Position* to, Party player);

/* Same check, as a yes/no answer. */
bool is_move_valid(const Board* board, const Position* from,
    const Position* to, Party player);

/* Applies every capture caused by the piece that has just moved to 'moved_to'.
 * Order: encirclement, then sandwich, then king capture.
 * Returns the number of pieces removed from the board. */
int check_capture(Board* board, const Position* moved_to);

/* Moves the piece. Does nothing if the move is not possible
 * (call check_move() first). */
void apply_move(Board* board, const Position* from, const Position* to);

/* 0: not over, 1: attackers win (king captured), 2: defenders win (king on an
 * edge), 3: draw (no defender left besides the king). */
int is_game_over(const Board* board);

#endif