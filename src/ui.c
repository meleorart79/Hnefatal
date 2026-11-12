
#include "ui.h"
#include <stdio.h>

#define RESET       "\x1b[0m"
#define BORDER      "\x1b[90m"
#define DEFENDER    "\x1b[94;1m"
#define ATTACKER    "\x1b[91;1m"
#define KING        "\x1b[96;1m"
#define HIGHLIGHT   "\x1b[43;1m"
#define CASTLE      "\x1b[33;1m"

void render_board(const Board* board, const Position* last_move)
{
    if (board == NULL)
        return;

    printf("\n");

    for (int row = ROW_COUNT - 1; row >= 0; row--) {
        printf(" %2d  ", row + 1);

        for (int col = 0; col < COLUMN_COUNT; col++) {
            const Cell* cell = &board->cells[row * COLUMN_COUNT + col];

            int highlighted = last_move != NULL &&
                last_move->row == row + 1 &&
                last_move->column == 'A' + col;

            char symbol = ' ';
            const char* foreground = RESET;

            if (cell->piece.party == PARTY_A && cell->piece.rank == RANK_P) {
                symbol = 'a';
                foreground = ATTACKER;
            }
            else if (cell->piece.party == PARTY_D && cell->piece.rank == RANK_P) {
                symbol = 'd';
                foreground = DEFENDER;
            }
            else if (cell->piece.party == PARTY_D && cell->piece.rank == RANK_K) {
                symbol = 'k';
                foreground = KING;
            }
            else if (cell->piece.party == PARTY_NONE && cell->type == Castle) {
                symbol = '+';
                foreground = CASTLE;
            }

            if (highlighted)
                printf("%s", HIGHLIGHT);

            printf("%s %c %s", foreground, symbol, RESET);

            if (col < COLUMN_COUNT - 1)
                printf("%s|%s", BORDER, RESET);
        }

        printf("\n");

        if (row > 0) {
            printf("     ");

            for (int col = 0; col < COLUMN_COUNT; col++) {
                printf("%s---%s", BORDER, RESET);

                if (col < COLUMN_COUNT - 1)
                    printf("%s+%s", BORDER, RESET);
            }

            printf("\n");
        }
    }

    printf("     ");

    for (int col = 0; col < COLUMN_COUNT; col++)
        printf(" %c  ", 'A' + col);

    printf("\n\n");
}
