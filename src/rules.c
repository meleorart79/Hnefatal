/*
 * rules.c - drop-in replacement for your rules.c.
 * Move check with error codes, captures, apply_move, is_game_over.
 *
 * COORDINATES
 *   A Position holds what the player typed: column 'A'-'K', row 1-11.
 *   Inside this file every function works with 0-based integers
 *   (row 0-10, column 0-10). The conversion is done once, in row_of()
 *   and col_of(), and only there.
 *
 * RULES IMPLEMENTED (see section 6 of the report)
 *   - Sandwich: the mover captures an enemy piece (never the king) held
 *     in a straight line between the moved piece and a friendly piece,
 *     or between the moved piece and the EMPTY castle.
 *   - Encirclement: the mover captures an enemy group (2+ pieces, connected
 *     orthogonally) when none of its pieces can move. A group that touches
 *     the board edge, or contains the king, is never captured this way.
 *   - King: captured when attackers occupy all four adjacent squares.
 *   - Encirclement is applied before sandwiches. Only the mover captures.
 *   - Board edges are never hostile.
 */
#include <stddef.h>
#include "rules.h"

#define DIRECTION_COUNT 4

  /* The four orthogonal steps: { row step, column step }. */
static const int DIRECTIONS[DIRECTION_COUNT][2] = {
    { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }
};

/* All the pieces of one side that touch each other. */
typedef struct {
    int cells[BOARD_SIZE];  /* board indexes of the pieces in the group */
    int size;
    bool touches_edge;      /* one piece is on an Edge cell             */
    bool can_move;          /* at least one piece has a legal move      */
} Group;


/* ================== small helpers ================== */

/* Convert to int first, then do ALL arithmetic on ints. Row and Column are
 * unsigned in gcc, so a difference computed on them (to->row - from->row)
 * wraps around to a huge number instead of becoming negative. */
static int row_of(const Position* pos) { return pos->row - 1; }
static int col_of(const Position* pos) { return pos->column - 'A'; }

static bool in_bounds(int row, int col)
{
    return row >= 0 && row < ROW_COUNT && col >= 0 && col < COLUMN_COUNT;
}

static int cell_index(int row, int col)
{
    return row * COLUMN_COUNT + col;
}

static Party opponent_of(Party party)
{
    return (party == PARTY_A) ? PARTY_D : PARTY_A;
}

/* -1, 0 or +1 depending on the sign of value. */
static int sign(int value)
{
    if (value > 0) {
        return 1;
    }
    if (value < 0) {
        return -1;
    }
    return 0;
}

static void remove_piece(Cell* cell)
{
    cell->piece.party = PARTY_NONE;
    cell->piece.rank = RANK_NONE;
    cell->piece.id = 0;
}


/* ================== move check ================== */

const char* move_error_message(MoveResult result)
{
    switch (result) {
    case MOVE_OK:                     return "Move accepted.";
    case MOVE_ERR_BAD_ARGUMENT:       return "Internal error: missing argument.";
    case MOVE_ERR_OUT_OF_BOARD:       return "That square is outside the board.";
    case MOVE_ERR_NO_PIECE:           return "There is no piece on the start square.";
    case MOVE_ERR_NOT_YOUR_PIECE:     return "That piece is not yours.";
    case MOVE_ERR_SAME_SQUARE:        return "The piece must move to another square.";
    case MOVE_ERR_NOT_STRAIGHT:       return "Pieces move along a row or a column only.";
    case MOVE_ERR_DESTINATION_OCCUPIED: return "The destination is occupied.";
    case MOVE_ERR_CASTLE_RESERVED:    return "Only the king may stop on the castle.";
    case MOVE_ERR_PATH_BLOCKED:       return "A piece or the castle is in the way.";
    }
    return "Unknown error.";
}

/* The real check, on 0-based coordinates. Also used by the capture code. */
static MoveResult check_move_rc(const Board* board, int from_row, int from_col,
    int to_row, int to_col, Party player)
{
    if (!in_bounds(from_row, from_col) || !in_bounds(to_row, to_col)) {
        return MOVE_ERR_OUT_OF_BOARD;
    }

    const Cell* start = &board->cells[cell_index(from_row, from_col)];
    const Cell* dest = &board->cells[cell_index(to_row, to_col)];

    if (start->piece.party == PARTY_NONE) {
        return MOVE_ERR_NO_PIECE;
    }
    if (start->piece.party != player) {
        return MOVE_ERR_NOT_YOUR_PIECE;
    }
    if (from_row == to_row && from_col == to_col) {
        return MOVE_ERR_SAME_SQUARE;
    }
    if (from_row != to_row && from_col != to_col) {
        return MOVE_ERR_NOT_STRAIGHT;
    }
    if (dest->piece.party != PARTY_NONE) {
        return MOVE_ERR_DESTINATION_OCCUPIED;
    }
    if (dest->type == Castle && start->piece.rank != RANK_K) {
        return MOVE_ERR_CASTLE_RESERVED;
    }
    if (player != PARTY_A && player != PARTY_D) {
        return MOVE_ERR_BAD_ARGUMENT; 
    }

    /* Walk from the start toward the destination, one square at a time.
     * Every step is -1, 0 or +1, so the walk cannot jump out of the board. */
    int row_step = sign(to_row - from_row);
    int col_step = sign(to_col - from_col);
    int row = from_row + row_step;
    int col = from_col + col_step;

    while (row != to_row || col != to_col) {
        const Cell* between = &board->cells[cell_index(row, col)];
        if (between->piece.party != PARTY_NONE || between->type == Castle) {
            return MOVE_ERR_PATH_BLOCKED;
        }
        row += row_step;
        col += col_step;
    }
    return MOVE_OK;
}

MoveResult check_move(const Board* board, const Position* from,
    const Position* to, Party player)
{
    if (board == NULL || from == NULL || to == NULL) {
        return MOVE_ERR_BAD_ARGUMENT;
    }
    return check_move_rc(board, row_of(from), col_of(from),
        row_of(to), col_of(to), player);
}

bool is_move_valid(const Board* board, const Position* from,
    const Position* to, Party player)
{
    return check_move(board, from, to, player) == MOVE_OK;
}


/* ================== captures ================== */

/* Can the square (row, col) be the far end of a sandwich for 'mover'?
 * Yes if it holds a friendly piece, or if it is the empty castle.
 * Off the board: no (board edges are not hostile). */
static bool is_sandwich_partner(const Board* board, int row, int col, Party mover)
{
    if (!in_bounds(row, col)) {
        return false;
    }

    const Cell* cell = &board->cells[cell_index(row, col)];

    if (cell->piece.party == PARTY_NONE) {
        return cell->type == Castle;
    }

    return cell->piece.party == mover;
}

/* Sandwich captures around the piece that has just moved to (row, col). */
static int capture_sandwiches(Board* board, int row, int col, Party mover)
{
    int captured = 0;

    for (int d = 0; d < DIRECTION_COUNT; d++) {
        int victim_row = row + DIRECTIONS[d][0];
        int victim_col = col + DIRECTIONS[d][1];
        int far_row = victim_row + DIRECTIONS[d][0];
        int far_col = victim_col + DIRECTIONS[d][1];

        if (!in_bounds(victim_row, victim_col)) {
            continue;
        }

        Cell* victim = &board->cells[cell_index(victim_row, victim_col)];

        if (victim->piece.party != opponent_of(mover)) {
            continue;
        }

        if (is_sandwich_partner(board, far_row, far_col, mover)) {
            remove_piece(victim);
            captured++;
        }
    }
    return captured;
}

/* Collects the group of 'side' that contains the piece at index 'start',
 * using a stack instead of recursion. Every piece is pushed at most once
 * (visited[] is set when it is pushed), so the stack never exceeds BOARD_SIZE. */
static void explore_group(const Board* board, int start, Party side,
    bool visited[BOARD_SIZE], Group* group)
{
    int stack[BOARD_SIZE];
    int top = 0;

    group->size = 0;
    group->touches_edge = false;
    group->can_move = false;

    stack[top++] = start;
    visited[start] = true;

    while (top > 0) {
        int index = stack[--top];
        const Cell* cell = &board->cells[index];
        int row = index / COLUMN_COUNT;
        int col = index % COLUMN_COUNT;

        group->cells[group->size++] = index;
        if (cell->type == Edge) {
            group->touches_edge = true;
        }

        for (int d = 0; d < DIRECTION_COUNT; d++) {
            int next_row = row + DIRECTIONS[d][0];
            int next_col = col + DIRECTIONS[d][1];

            if (!in_bounds(next_row, next_col)) {
                continue;
            }

            int next = cell_index(next_row, next_col);

            if (board->cells[next].piece.party == side) {
                /* same side and touching: part of the group */
                if (!visited[next]) {
                    visited[next] = true;
                    stack[top++] = next;
                }
            }
            else if (check_move_rc(board, row, col, next_row, next_col, side) == MOVE_OK) {
                /* A rook-like piece can move only if it can step on a
                 * neighbouring square, so testing the 4 neighbours is the same as
                 * testing every destination (as long as nobody crosses the castle). */
                group->can_move = true;
            }
        }
    }
}

/* Encirclement: only the groups touching the piece that has just moved
 * can have become enclosed by this move. */
static int capture_encircled_groups(Board* board, int row, int col, Party mover)
{
    Party side = opponent_of(mover);
    bool visited[BOARD_SIZE] = { false };
    Group group;
    int captured = 0;

    for (int d = 0; d < DIRECTION_COUNT; d++) {
        int next_row = row + DIRECTIONS[d][0];
        int next_col = col + DIRECTIONS[d][1];

        if (!in_bounds(next_row, next_col)) {
            continue;
        }

        int start = cell_index(next_row, next_col);

        if (board->cells[start].piece.party != side || visited[start]) {
            continue;
        }

        explore_group(board, start, side, visited, &group);

        if (group.size >= 2 && !group.touches_edge && !group.can_move) {
            for (int i = 0; i < group.size; i++) {
                remove_piece(&board->cells[group.cells[i]]);
            }
            captured += group.size;
        }
    }
    return captured;
}

int check_capture(Board* board, const Position* moved_to)
{
    if (board == NULL || moved_to == NULL) {
        return 0;
    }

    int row = row_of(moved_to);
    int col = col_of(moved_to);

    if (!in_bounds(row, col)) {
        return 0;
    }

    Party mover = board->cells[cell_index(row, col)].piece.party;

    if (mover == PARTY_NONE) {
        return 0;
    }

    int captured = 0;

    captured += capture_encircled_groups(board, row, col, mover);
    captured += capture_sandwiches(board, row, col, mover);
    return captured;
}


/* ================== move and end of game ================== */

void apply_move(Board* board, const Position* from, const Position* to)
{
    if (board == NULL || from == NULL || to == NULL)
        return;

    int from_row = row_of(from);
    int from_col = col_of(from);
    int to_row = row_of(to);
    int to_col = col_of(to);

    if (!in_bounds(from_row, from_col) || !in_bounds(to_row, to_col))
        return;

    Cell* start = &board->cells[cell_index(from_row, from_col)];

    if (check_move_rc(board, from_row, from_col,
        to_row, to_col, start->piece.party) != MOVE_OK)
        return;

    Cell* dest = &board->cells[cell_index(to_row, to_col)];

    dest->piece = start->piece;
    remove_piece(start);
}

int is_game_over(const Board* board)
{
    /* 0: not over, 1: attackers win, 2: defenders win, 3: draw */
    if (board == NULL) {
        return 0;
    }

    bool king_found = false;
    bool king_on_edge = false;
    int defenders = 0, attackers = 0;              /* defender pawns, the king is not counted */

    for (int i = 0; i < BOARD_SIZE; i++) {
        const Cell* cell = &board->cells[i];

        if (cell->piece.party == PARTY_NONE) {
            continue;
        }
        if (cell->piece.rank == RANK_K) {
            king_found = true;
            if (cell->type == Edge) {
                king_on_edge = true;
            }
        }
        else if (cell->piece.party == PARTY_D) {
            defenders++;
        }
        else if (cell->piece.party == PARTY_A) {
            attackers++;
        }
    }

    if (!king_found) {
        return 1;
    }
    if (king_on_edge) {
        return 2;
    }
    if (defenders == 0) {
        return 3;
    }
	if (attackers == 0) {
		return 2;
	}
    return 0;
}