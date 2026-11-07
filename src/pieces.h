
#ifndef PIECES_H
#define PIECES_H

typedef enum { 
    PARTY_D, PARTY_A, 
    PARTY_COUNT
} Party;

typedef enum {
    RANK_P, RANK_K,
    RANK_COUNT
} Rank;

#define INIT_PIECE_COUNT 37

typedef struct {
    Party party;
    Rank rank;
    int id;
} Piece;

typedef struct {
    Piece pieces[INIT_PIECE_COUNT];
	int a_count;
	int d_count;
    int  count;          
} BoardP;

const char* rank_name(Rank rank);
const char* party_name(Party party);
void piece_print(Piece piece);
BoardP boardp_make(void);     

#endif

