
#include <assert.h>
#include <stdio.h>

#include "rules.h"

#define CELL(b, col, row) \
    (&(b).cells[((row) - 1) * COLUMN_COUNT + ((col) - 'A')])

int main(void)
{
    Board board = board_make();

    Position from = { 2, 'B' };
    Position to = { 5, 'B' };

    // Place an attacking pawn at B2
    CELL(board, 'B', 2)->piece = (Piece){ PARTY_A, RANK_P, 101 };
    assert(CELL(board, 'B', 2)->piece.party != PARTY_NONE);

    // Valid straight-line movement
    assert(is_move_valid(&board, &from, &to, PARTY_A));

    // Wrong player's turn
    assert(!is_move_valid(&board, &from, &to, PARTY_D));

    // Diagonal movement
    Position diagonal = { 5, 'C' };
    assert(!is_move_valid(&board, &from, &diagonal, PARTY_A));

    // Blocked path
    CELL(board, 'B', 4)->piece.party = PARTY_D;
    assert(!is_move_valid(&board, &from, &to, PARTY_A));

    // Clear path again
    CELL(board, 'B', 4)->piece.party = PARTY_NONE;
    assert(is_move_valid(&board, &from, &to, PARTY_A));

    // Apply movement
    apply_move(&board, &from, &to);

    assert(CELL(board, 'B', 2)->piece.party == PARTY_NONE);
    assert(CELL(board, 'B', 5)->piece.party != PARTY_NONE);
    assert(CELL(board, 'B', 5)->piece.id == 101);

    printf("test_rules: OK\n");

    return 0;
}
