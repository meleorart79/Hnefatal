
#include "game.h"
#include <stdio.h>

const char* game_title(void) { return "HNEFATAL"; }

const char* game_version(void) { return "0.1.0"; }

/* AI-assisted: FEN-like starting position. Digits represent consecutive empty cells. */
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

            board.cells[i].occupied = false;
            i++;
        }
    }

    return board;
}

static Cell* board_cell(Board* board, char column, int row)
{
    int c = column - 'A';
    int r = row - 1;

    return &board->cells[r * COLUMN_COUNT + c];
}

/* AI-assisted: Converts a FEN-like position string into a board. */
bool board_position_from_string(Board* board, const char* position)
{
    if (board == NULL || position == NULL)
        return false;

    Board parsed = board_make();
    int row = 0;
    int column = 0;
    int piece_id = 0;
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
            cell->piece = (Piece){ PARTY_A, RANK_P, piece_id++ };
            cell->occupied = true;
            break;

        case 'd':
            cell->piece = (Piece){ PARTY_D, RANK_P, piece_id++ };
            cell->occupied = true;
            break;

        case 'k':
            cell->piece = (Piece){ PARTY_D, RANK_K, piece_id++ };
            cell->occupied = true;
            king_count++;
            break;

        default:
            return false;
        }

        column++;
    }

    if (row != ROW_COUNT - 1 ||
        column != COLUMN_COUNT ||
        king_count != 1 ||
        piece_id > INIT_PIECE_COUNT)
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

            if (!cell->occupied) {
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

        if (row > 0)
            position[index++] = '/';
    }

    position[index] = '\0';
    return king_count == 1;
}

// AI-assisted: Sets up the initial board using the position string.
void board_setup(Board* board)
{
    board_position_from_string(board, START_POSITION);
}

void game_init(GameState* game)
{
    game->board = board_make();
    board_setup(&game->board);
}

