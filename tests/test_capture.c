/*
 * test_capture.c - tests for check_move() and check_capture().
 * Build: gcc -std=c11 -Wall -Wextra -Isrc tests/test_capture.c src/board.c src/rules.c -o build/test_capture
 * 
 */
#include <assert.h>
#include <stdio.h>

#include "rules.h"

/* ---------- helpers: build positions on an empty board ---------- */

static Position at(char column, int row)
{
    Position pos;
    pos.column = column;
    pos.row = row;
    return pos;
}

static Cell* cell(Board* board, char column, int row)
{
    return &board->cells[(row - 1) * COLUMN_COUNT + (column - 'A')];
}

static void put(Board* board, char column, int row, Party party, Rank rank)
{
    cell(board, column, row)->piece.party = party;
    cell(board, column, row)->piece.rank = rank;
    cell(board, column, row)->piece.id = 1;
}

static void put_attacker(Board* board, char column, int row) { put(board, column, row, PARTY_A, RANK_P); }
static void put_defender(Board* board, char column, int row) { put(board, column, row, PARTY_D, RANK_P); }
static void put_king(Board* board, char column, int row)     { put(board, column, row, PARTY_D, RANK_K); }

static bool is_empty(Board* board, char column, int row)
{
    return cell(board, column, row)->piece.party == PARTY_NONE;
}

static MoveResult move(const Board* board, char c1, int r1, char c2, int r2, Party player)
{
    Position from = at(c1, r1);
    Position to = at(c2, r2);
    return check_move(board, &from, &to, player);
}

static int capture(Board* board, char column, int row)
{
    Position pos = at(column, row);
    return check_capture(board, &pos);
}

/* ---------- move check ---------- */

static void test_moves_in_all_directions(void)
{
    Board board = board_make();
    put_attacker(&board, 'E', 5);

    /* includes left and down moves of 2 squares or more (the old crash) */
    assert(move(&board, 'E', 5, 'E', 8, PARTY_A) == MOVE_OK);   /* up    3 */
    assert(move(&board, 'E', 5, 'E', 2, PARTY_A) == MOVE_OK);   /* down  3 */
    assert(move(&board, 'E', 5, 'H', 5, PARTY_A) == MOVE_OK);   /* right 3 */
    assert(move(&board, 'E', 5, 'B', 5, PARTY_A) == MOVE_OK);   /* left  3 */
    assert(move(&board, 'E', 5, 'E', 4, PARTY_A) == MOVE_OK);   /* down  1 */
    assert(move(&board, 'E', 5, 'D', 5, PARTY_A) == MOVE_OK);   /* left  1 */
}

static void test_move_errors(void)
{
    Board board = board_make();
    put_attacker(&board, 'B', 2);
    put_attacker(&board, 'B', 5);
    put_defender(&board, 'E', 2);
    put_attacker(&board, 'F', 9);
    put_king(&board, 'F', 8);

    Position any = at('A', 1);
    assert(check_move(NULL, &any, &any, PARTY_A) == MOVE_ERR_BAD_ARGUMENT);
    assert(move(&board, 'B', 2, 'L', 2, PARTY_A) == MOVE_ERR_OUT_OF_BOARD);
    assert(move(&board, 'B', 2, 'B', 12, PARTY_A) == MOVE_ERR_OUT_OF_BOARD);
    assert(move(&board, 'B', 2, 'B', 0, PARTY_A) == MOVE_ERR_OUT_OF_BOARD);
    assert(move(&board, 'C', 3, 'C', 4, PARTY_A) == MOVE_ERR_NO_PIECE);
    assert(move(&board, 'E', 2, 'E', 3, PARTY_A) == MOVE_ERR_NOT_YOUR_PIECE);
    assert(move(&board, 'B', 2, 'B', 2, PARTY_A) == MOVE_ERR_SAME_SQUARE);
    assert(move(&board, 'B', 2, 'C', 3, PARTY_A) == MOVE_ERR_NOT_STRAIGHT);
    assert(move(&board, 'B', 2, 'B', 5, PARTY_A) == MOVE_ERR_DESTINATION_OCCUPIED);
    assert(move(&board, 'B', 2, 'B', 7, PARTY_A) == MOVE_ERR_PATH_BLOCKED);       /* B5 in the way  */
    assert(move(&board, 'F', 9, 'F', 6, PARTY_A) == MOVE_ERR_CASTLE_RESERVED);    /* ends on castle */
    assert(move(&board, 'F', 9, 'F', 3, PARTY_A) == MOVE_ERR_PATH_BLOCKED);       /* king in the way */
}

static void test_castle(void)
{
    Board board = board_make();
    put_attacker(&board, 'F', 9);
    put_king(&board, 'F', 8);

    /* the king may stop on the castle ... */
    assert(move(&board, 'F', 8, 'F', 6, PARTY_D) == MOVE_OK);
    /* ... but nobody crosses it */
    board = board_make();
    put_attacker(&board, 'F', 9);
    assert(move(&board, 'F', 9, 'F', 3, PARTY_A) == MOVE_ERR_PATH_BLOCKED);
}

/* ---------- sandwich ---------- */

static void test_sandwich(void)
{
    Board board = board_make();

    /* basic capture: attacker arrives on C5, defender D5, attacker E5 */
    put_attacker(&board, 'E', 5);
    put_defender(&board, 'D', 5);
    put_attacker(&board, 'C', 5);
    assert(capture(&board, 'C', 5) == 1);
    assert(is_empty(&board, 'D', 5));
    assert(!is_empty(&board, 'C', 5));

    /* far square empty: no capture */
    board = board_make();
    put_defender(&board, 'D', 5);
    put_attacker(&board, 'C', 5);
    assert(capture(&board, 'C', 5) == 0);
    assert(!is_empty(&board, 'D', 5));

    /* far square off the board: edges are not hostile */
    board = board_make();
    put_defender(&board, 'A', 5);
    put_attacker(&board, 'B', 5);
    assert(capture(&board, 'B', 5) == 0);
    assert(!is_empty(&board, 'A', 5));

    /* empty castle is hostile */
    board = board_make();
    put_defender(&board, 'F', 7);
    put_attacker(&board, 'F', 8);
    assert(capture(&board, 'F', 8) == 1);
    assert(is_empty(&board, 'F', 7));

    /* castle occupied by the king is not hostile to attackers */
    board = board_make();
    put_king(&board, 'F', 6);
    put_defender(&board, 'F', 7);
    put_attacker(&board, 'F', 8);
    assert(capture(&board, 'F', 8) == 0);
    assert(!is_empty(&board, 'F', 7));
}

static void test_sandwich_exceptions(void)
{
    Board board = board_make();

    /* moving between two enemies is safe, and captures nothing */
    put_defender(&board, 'C', 5);
    put_defender(&board, 'E', 5);
    put_attacker(&board, 'D', 5);
    assert(capture(&board, 'D', 5) == 0);
    assert(!is_empty(&board, 'D', 5));

    /* the king can be captured by a normal sandwich */
    board = board_make();
    put_king(&board, 'E', 5);
    put_attacker(&board, 'D', 5);
    put_attacker(&board, 'F', 5);
    assert(capture(&board, 'F', 5) == 1);
    assert(is_empty(&board, 'E', 5));

    /* own pieces are never captured */
    board = board_make();
    put_attacker(&board, 'D', 5);
    put_defender(&board, 'E', 5);
    put_attacker(&board, 'C', 5);
    assert(capture(&board, 'C', 5) == 0);
    assert(!is_empty(&board, 'D', 5));

    /* defenders capture too, with the king as the second piece */
    board = board_make();
    put_king(&board, 'E', 5);
    put_attacker(&board, 'D', 5);
    put_defender(&board, 'C', 5);
    assert(capture(&board, 'C', 5) == 1);
    assert(is_empty(&board, 'D', 5));
}

/* ---------- encirclement ---------- */

static void test_encirclement(void)
{
    Board board = board_make();

    /* pair C3-D3 closed by the last attacker arriving on D4.
     * D3 would also be sandwiched (D2), but encirclement goes first:
     * the result must be 2 pieces, not 3. */
    put_defender(&board, 'C', 3);
    put_defender(&board, 'D', 3);
    put_attacker(&board, 'B', 3);
    put_attacker(&board, 'E', 3);
    put_attacker(&board, 'C', 2);
    put_attacker(&board, 'D', 2);
    put_attacker(&board, 'C', 4);
    put_attacker(&board, 'D', 4);
    assert(capture(&board, 'D', 4) == 2);
    assert(is_empty(&board, 'C', 3));
    assert(is_empty(&board, 'D', 3));

    /* one neighbour left open: nothing is captured */
    board = board_make();
    put_defender(&board, 'C', 3);
    put_defender(&board, 'D', 3);
    put_attacker(&board, 'B', 3);
    put_attacker(&board, 'E', 3);
    put_attacker(&board, 'C', 2);
    put_attacker(&board, 'C', 4);
    put_attacker(&board, 'D', 4);   /* D2 stays empty */
    assert(capture(&board, 'D', 4) == 0);
    assert(!is_empty(&board, 'C', 3));
    assert(!is_empty(&board, 'D', 3));

    /* L-shape of three */
    board = board_make();
    put_defender(&board, 'C', 3);
    put_defender(&board, 'D', 3);
    put_defender(&board, 'D', 4);
    put_attacker(&board, 'B', 3);
    put_attacker(&board, 'C', 2);
    put_attacker(&board, 'D', 2);
    put_attacker(&board, 'E', 3);
    put_attacker(&board, 'C', 4);
    put_attacker(&board, 'E', 4);
    put_attacker(&board, 'D', 5);
    assert(capture(&board, 'D', 5) == 3);
    assert(is_empty(&board, 'C', 3) && is_empty(&board, 'D', 3) && is_empty(&board, 'D', 4));

    /* the castle counts as a wall for pieces that are not the king */
    board = board_make();
    put_defender(&board, 'F', 7);
    put_defender(&board, 'G', 7);
    put_attacker(&board, 'E', 7);
    put_attacker(&board, 'F', 8);
    put_attacker(&board, 'G', 8);
    put_attacker(&board, 'H', 7);
    put_attacker(&board, 'G', 6);
    assert(capture(&board, 'E', 7) == 2);
    assert(is_empty(&board, 'F', 7) && is_empty(&board, 'G', 7));
}

static void test_encirclement_exceptions(void)
{
    Board board = board_make();

    /* a group touching the edge is never captured */
    put_defender(&board, 'A', 3);
    put_defender(&board, 'B', 3);
    put_attacker(&board, 'A', 2);
    put_attacker(&board, 'A', 4);
    put_attacker(&board, 'B', 2);
    put_attacker(&board, 'B', 4);
    put_attacker(&board, 'C', 3);
    assert(capture(&board, 'C', 3) == 0);
    assert(!is_empty(&board, 'A', 3) && !is_empty(&board, 'B', 3));

    /* a group containing the king can be captured by encirclement */
    board = board_make();
    put_king(&board, 'C', 3);
    put_defender(&board, 'D', 3);
    put_attacker(&board, 'B', 3);
    put_attacker(&board, 'C', 2);
    put_attacker(&board, 'C', 4);
    put_attacker(&board, 'D', 2);
    put_attacker(&board, 'D', 4);
    put_attacker(&board, 'E', 3);
    assert(capture(&board, 'E', 3) == 2);
    assert(is_empty(&board, 'C', 3) && is_empty(&board, 'D', 3));

    /* the mover's own enclosed group stays: only the mover captures */
    board = board_make();
    put_attacker(&board, 'C', 3);
    put_attacker(&board, 'D', 3);
    put_defender(&board, 'B', 3);
    put_defender(&board, 'E', 3);
    put_defender(&board, 'C', 2);
    put_defender(&board, 'D', 2);
    put_defender(&board, 'C', 4);
    put_defender(&board, 'D', 4);
    put_attacker(&board, 'I', 9);
    assert(capture(&board, 'I', 9) == 0);
    assert(!is_empty(&board, 'C', 3) && !is_empty(&board, 'D', 3));

    /* defenders can encircle an attacker group too */
    put_defender(&board, 'D', 4);
    assert(capture(&board, 'D', 4) == 2);
    assert(is_empty(&board, 'C', 3) && is_empty(&board, 'D', 3));
}

/* ---------- apply_move and end of game ---------- */

static void test_apply_and_outcome(void)
{
    Board board = board_make();
    board_init(&board);
    assert(is_game_over(&board) == 0);                 /* start position */

    Position from = at('F', 2);
    Position to = at('F', 3);
    assert(check_move(&board, &from, &to, PARTY_A) == MOVE_OK);
    apply_move(&board, &from, &to);
    assert(is_empty(&board, 'F', 2) && !is_empty(&board, 'F', 3));

    board = board_make();                              /* no king */
    put_attacker(&board, 'E', 5);
    assert(is_game_over(&board) == 1);

    board = board_make();                              /* king on an edge */
    put_king(&board, 'A', 6);
    put_defender(&board, 'C', 3);
    assert(is_game_over(&board) == 2);

    board = board_make();                              /* only the king left */
    put_king(&board, 'E', 5);
    put_attacker(&board, 'C', 3);
    assert(is_game_over(&board) == 3);

    board = board_make();                              /* game goes on */
    put_king(&board, 'E', 5);
    put_defender(&board, 'C', 3);
    put_attacker(&board, 'I', 9);
    assert(is_game_over(&board) == 0);
}

int main(void)
{
    test_moves_in_all_directions();
    test_move_errors();
    test_castle();
    test_sandwich();
    test_sandwich_exceptions();
    test_encirclement();
    test_encirclement_exceptions();
    test_apply_and_outcome();

    printf("test_capture: OK\n");
    return 0;
}
