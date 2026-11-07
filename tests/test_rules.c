#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "rules.h"

static piece C(Suit s, Rank r) { piece c = { s, r }; return c; }

int main(void)
{
    piece four[4] = { C(SUIT_SPADES, RANK_ACE), C(SUIT_HEARTS, RANK_ACE),
                      C(SUIT_DIAMONDS, RANK_ACE), C(SUIT_CLUBS, RANK_ACE) };

    piece three[4] = { C(SUIT_SPADES, RANK_ACE), C(SUIT_HEARTS, RANK_ACE),
                      C(SUIT_DIAMONDS, RANK_ACE), C(SUIT_CLUBS, RANK_KING) };

    piece four_diff_order[4] = { C(SUIT_DIAMONDS, RANK_7), C(SUIT_CLUBS, RANK_7), 
                      C(SUIT_SPADES, RANK_7), C(SUIT_HEARTS, RANK_7) };

    piece same_color[4] = { C(SUIT_SPADES, RANK_7), C(SUIT_SPADES, RANK_8),
                      C(SUIT_SPADES, RANK_9), C(SUIT_SPADES, RANK_10) };

    piece n_three[3] = { C(SUIT_SPADES, RANK_ACE), C(SUIT_HEARTS, RANK_ACE),
                      C(SUIT_DIAMONDS, RANK_ACE) };

    piece n_none[0] = {};

    int y = 0;
	int* x = &y;

    printf("Cas: AP AC AD AT, Attendu: KEMS\n");
	printf("Obtenu: %s, %s\n", rules_is_four_of_a_kind(four, 4) ? "KEMS" : "pas KEMS", rank_name(rules_best_rank(four, 4, x)));

    printf("Cas: 7D 7T 7P 7C , Attendu: KEMS\n");
    printf("Obtenu: %s, %s\n", rules_is_four_of_a_kind(four_diff_order, 4) ? "KEMS" : "pas KEMS", rank_name(rules_best_rank(four_diff_order, 4, x)));

    printf("Cas: AP AC AD RT, Attendu: Pas KEMS\n");
    printf("Obtenu: %s, %s\n", rules_is_four_of_a_kind(three, 4) ? "KEMS" : "pas KEMS", rank_name(rules_best_rank(three, 4, x)));

    printf("Cas: 7P 8P 9P 10P, Attendu: Pas KEMS\n");
    printf("Obtenu: %s, %s\n", rules_is_four_of_a_kind(same_color, 4) ? "KEMS" : "pas KEMS", rank_name(rules_best_rank(same_color, 4, x)));

    printf("Cas: AP AC AD, Attendu: Pas KEMS\n");
    printf("Obtenu: %s, %s\n", rules_is_four_of_a_kind(n_three, 3) ? "KEMS" : "pas KEMS", rank_name(rules_best_rank(n_three, 3, x)));

    printf("Cas: Pas de cartes, Attendu: Pas KEMS\n");
    printf("Obtenu: %s, %s\n", rules_is_four_of_a_kind(n_none, 0) ? "KEMS" : "pas KEMS", rank_name(rules_best_rank(n_none, 0, x)));
    printf("-------\n");

    /* Simulation : un KEMS direct dans la main de depart */
    srand(42);
    const int trials = 200000;
    int hits = 0;
    for (int t = 0; t < trials; t++) {
        BoardP d = boardp_make();
        boardp_shuffle(&d);
        piece hand[4];
        for (int i = 0; i < 4; i++) hand[i] = boardp_draw(&d);
        if (rules_is_four_of_a_kind(hand, 4)) hits++;
    }
    printf("KEMS direct : %d sur %d (attendu environ 9.6)\n", hits, trials);
    assert(hits >= 1 && hits <= 25);

    printf("test_rules : OK\n");
    return 0;
}