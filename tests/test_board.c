
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "board.h"

int main(void)
{
    Board board = board_make();

    // Board dimensions and cell types
    assert(board.cells[0].type == Edge);
    assert(board.cells[5 * COLUMN_COUNT + 5].type == Castle);
    assert(board.cells[1 * COLUMN_COUNT + 1].type == Open);

    // Load a simple position containing one king
    const char* position =
        "11/11/11/11/11/5k5/11/11/11/11/11";

    assert(board_position_from_string(&board, position));

    // Verify king placement
    Cell* king = &board.cells[5 * COLUMN_COUNT + 5];

    assert(king->piece.party != PARTY_NONE);
    assert(king->piece.rank == RANK_K);
    assert(king->piece.party == PARTY_D);

    // Serialize and compare
    char result[POSITION_STRING_SIZE];

    assert(board_position_to_string(&board, result));
    assert(strcmp(position, result) == 0);

    // Reject invalid positions
    assert(!board_position_from_string(&board, NULL));

    printf("test_board: OK\n");

    return 0;
}
