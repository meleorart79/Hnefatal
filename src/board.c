#include "board.h"
#include <stdio.h>

static const char* START_POSITION = "3aaaaa3/5a5/11/a4d4a/a3ddd3a/aa1ddkdd1aa/a3ddd3a/a4d4a/11/5a5/3aaaaa3";

Board board_make(void)
{
    Board board = { 0 };
    int i = 0;

    for (Row row = 0; row < ROW_COUNT; row++) {
        for (Column column = 0; column < COLUMN_COUNT; column++) {
            board.cells[i].row = row;
            board.cells[i].column = column;

            if (row == ROW_1 || row == ROW_11 || column == COLUMN_A || column == COLUMN_K) {
                board.cells[i].type = Edge;
            }
            else if (row == ROW_6 && column == COLUMN_F) {
                board.cells[i].type = Castle;
            }
            else {
                board.cells[i].type = Open;
            }

            board.cells[i].piece.party = PARTY_NONE;
            board.cells[i].piece.rank = RANK_NONE;
            board.cells[i].piece.id = 0;
            i++;
        }
    }

    return board;
}

bool board_position_from_string(Board* board, const char* position)
{
    if (board == NULL || position == NULL)
        return false;

    Board parsed = board_make();
    int row = 0;
    int column = 0;
    int a_piece_id = 1;
    int d_piece_id = 1;
    int king_count = 0;

    for (int i = 0; position[i] != '\0'; i++) {
        char symbol = position[i];

        if (symbol >= '0' && symbol <= '9') {
            int empty_count = 0;

            while (position[i] >= '0' && position[i] <= '9') {
                empty_count = empty_count * 10 + position[i] - '0';
                i++;
            }
            i--;

            if (empty_count == 0 || column + empty_count > COLUMN_COUNT)
                return false;

            column += empty_count;
            continue;
        }

        if (symbol == '/') {
            if (column != COLUMN_COUNT || row >= ROW_COUNT - 1)
                return false;

            row++;
            column = 0;
            continue;
        }

        if (column >= COLUMN_COUNT || row >= ROW_COUNT)
            return false;

        Cell* cell = &parsed.cells[
            (ROW_COUNT - 1 - row) * COLUMN_COUNT + column
        ];

        switch (symbol) {
        case 'a':
            if (cell->type == Castle)
                return false;

            cell->piece = (Piece){ PARTY_A, RANK_P, 100 + a_piece_id++ };
            break;

        case 'd':
            if (cell->type == Castle)
                return false;

            cell->piece = (Piece){ PARTY_D, RANK_P, 200 + d_piece_id++ };
            break;

        case 'k':
            if (king_count >= 1)
                return false;

            cell->piece = (Piece){ PARTY_D, RANK_K, 200 };
            king_count++;
            break;

        default:
            return false;
        }

        column++;
    }

    if (row != ROW_COUNT - 1 || column != COLUMN_COUNT)
        return false;

    *board = parsed;
    return true;
}

/* AI-assisted: Converts a board into a compressed FEN-like position string. */
bool board_position_to_string(const Board* board, char position[POSITION_STRING_SIZE])
{
    if (board == NULL || position == NULL)
        return false;

    int index = 0;
    int king_count = 0;

    for (int row = ROW_COUNT - 1; row >= 0; row--) {
        int empty_count = 0;

        for (int column = 0; column < COLUMN_COUNT; column++) {
            const Cell* cell = &board->cells[
                row * COLUMN_COUNT + column
            ];

            if (cell->piece.party == PARTY_NONE) {
                if (cell->piece.rank != RANK_NONE)
                    return false;

                empty_count++;
                continue;
            }

            if (empty_count > 0) {
                int written = snprintf(
                    position + index,
                    POSITION_STRING_SIZE - index,
                    "%d",
                    empty_count
                );

                if (written < 0 || written >= POSITION_STRING_SIZE - index)
                    return false;

                index += written;
                empty_count = 0;
            }

            Piece piece = cell->piece;

            if (cell->type == Castle &&
                !(cell->piece.party == PARTY_D && cell->piece.rank == RANK_K))
                return false;

            if (index >= POSITION_STRING_SIZE - 1)
                return false;

            if (piece.party == PARTY_A && piece.rank == RANK_P) {
                position[index++] = 'a';
            }
            else if (piece.party == PARTY_D && piece.rank == RANK_P) {
                position[index++] = 'd';
            }
            else if (piece.party == PARTY_D && piece.rank == RANK_K) {
                position[index++] = 'k';
                king_count++;
            }
            else {
                return false;
            }
        }

        if (empty_count > 0) {
            int written = snprintf(
                position + index,
                POSITION_STRING_SIZE - index,
                "%d",
                empty_count
            );

            if (written < 0 || written >= POSITION_STRING_SIZE - index)
                return false;

            index += written;
        }

        if (row > 0) {
            if (index >= POSITION_STRING_SIZE - 1)
                return false;

            position[index++] = '/';
        }
    }

    position[index] = '\0';
    return king_count <= 1;
}

bool board_setup(Board* board)
{
    if (board == NULL)
        return false;

    return board_position_from_string(board, START_POSITION);
}

bool board_init(Board* board)
{
    if (board == NULL)
        return false;

    *board = board_make();

    return board_setup(board);
}

