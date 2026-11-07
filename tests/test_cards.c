#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"

static bool all_unique(const piece* pieces, int n)
{
    bool seen[SUIT_COUNT][RANK_COUNT] = { false };
    for (int i = 0; i < n; i++) {
        if (seen[pieces[i].suit][pieces[i].rank]) return false;
        seen[pieces[i].suit][pieces[i].rank] = true;
    }
    return true;
}



int main(void)
{
    srand(42);
    GameState game;
    game_init(&game);
    assert(all_unique(game.boardp.pieces, game.boardp.count));
    printf("test_pieces : OK\n");
    return 0;
}
