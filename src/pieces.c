
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "pieces.h"

static const char* PARTY_NAMES[PARTY_COUNT] = {
    [PARTY_D] = "Defender", [PARTY_A] = "Attacker"
};

static const char* RANK_NAMES[RANK_COUNT] = {
    [RANK_P] = "Pawn", [RANK_K] = "King"
};

const char* party_name(Party party) { return PARTY_NAMES[party]; }
const char* rank_name(Rank rank) { return RANK_NAMES[rank]; }

void piece_print(Piece piece)
{
    printf("%s %s\n", rank_name(piece.rank), party_name(piece.party));
}

BoardP boardp_make(void)
{
    BoardP boardp;
    boardp.count = 0;
    boardp.a_count = 0;
    boardp.d_count = 0;
	for (Party party = 0; party < PARTY_COUNT; party++) {

		if (party == PARTY_D) {
			while (boardp.d_count < 12) {
				boardp.pieces[boardp.count].party = party;
				boardp.pieces[boardp.count].rank = RANK_P;
				boardp.pieces[boardp.count].id = boardp.count;
				boardp.count++;
				boardp.d_count++;
			}
			boardp.pieces[boardp.count].party = party;
			boardp.pieces[boardp.count].rank = RANK_K;
			boardp.pieces[boardp.count].id = boardp.count;
			boardp.count++;
			boardp.d_count++;
		}

		if (party == PARTY_A) {
			while (boardp.a_count < 24) {
				boardp.pieces[boardp.count].party = party;
				boardp.pieces[boardp.count].rank = RANK_P;
				boardp.pieces[boardp.count].id = boardp.count;
				boardp.count++;
				boardp.a_count++;
			}
		}
	}
	
    return boardp;
} 

