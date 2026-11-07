
#include <stddef.h>
#include "rules.h"

bool is_move_valid(const Board* board, const Position* from, const Position* to)
{
	if (board == NULL || from == NULL || to == NULL)
		return false;
	// Check if the move is on the same row or column
	if (from->row != to->row && from->column != to->column)
		return false;
	// Check if the path is clear
	int row_step = (to->row - from->row) ? ((to->row - from->row) / abs(to->row - from->row)) : 0;
	int col_step = (to->column - from->column) ? ((to->column - from->column) / abs(to->column - from->column)) : 0;
	int current_row = from->row + row_step;
	int current_col = from->column + col_step;
	while (current_row != to->row || current_col != to->column) {
		const Cell* cell = &board->cells[(current_row - 1) * COLUMN_COUNT + (current_col - 'A')];
		if (cell->occupied)
			return false;
		current_row += row_step;
		current_col += col_step;
	}
	return true;
}

bool is_capture_valid(const Board* board, const Position* pos)
{
	if (board == NULL || pos == NULL)
		return false;
	// Check adjacent cells for enemy pieces
	int directions[4][2] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };
	for (int i = 0; i < 4; i++) {
		int adj_row = pos->row + directions[i][0];
		int adj_col = pos->column + directions[i][1];
		if (adj_row >= 1 && adj_row <= ROW_COUNT && adj_col >= 'A' && adj_col <= 'K') {
			const Cell* cell = &board->cells[(adj_row - 1) * COLUMN_COUNT + (adj_col - 'A')];
			if (cell->occupied && cell->piece.party != board->cells[(pos->row - 1) * COLUMN_COUNT + (pos->column - 'A')].piece.party) {
				// Check if the enemy piece is surrounded
				int surround_count = 0;
				for (int j = 0; j < 4; j++) {
					int check_row = adj_row + directions[j][0];
					int check_col = adj_col + directions[j][1];
					if (check_row >= 1 && check_row <= ROW_COUNT && check_col >= 'A' && check_col <= 'K') {
						const Cell* check_cell = &board->cells[(check_row - 1) * COLUMN_COUNT + (check_col - 'A')];
						if (!check_cell->occupied || check_cell->piece.party == board->cells[(pos->row - 1) * COLUMN_COUNT + (pos->column - 'A')].piece.party) {
							surround_count++;
						}
					}
				}
				if (surround_count >= 2)
					return true;
			}
		}
	}
	return false;
}

bool is_game_over(const Board* board)
{
	if (board == NULL)
		return false;
	// Check if the king is captured
	for (int i = 0; i < BOARD_SIZE; i++) {
		const Cell* cell = &board->cells[i];
		if (cell->occupied && cell->piece.rank == RANK_K) {
			return false; // King is still on the board
		}
	}
	return true; // King is captured, game over
}

void apply_move(Board* board, const Position* from, const Position* to)
{
	if (board == NULL || from == NULL || to == NULL)
		return;
	Cell* from_cell = &board->cells[(from->row - 1) * COLUMN_COUNT + (from->column - 'A')];
	Cell* to_cell = &board->cells[(to->row - 1) * COLUMN_COUNT + (to->column - 'A')];
	if (from_cell->occupied && !to_cell->occupied) {
		to_cell->piece = from_cell->piece;
		to_cell->occupied = true;
		from_cell->occupied = false;
	}
}

