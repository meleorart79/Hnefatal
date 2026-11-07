
#ifndef RULES_H
#define RULES_H

#include <stdbool.h>
#include "pieces.h"

bool is_move_valid(const Board* board, const Position* from, const Position* to);
bool is_capture_valid(const Board* board, const Position* pos);
bool is_game_over(const Board* board);
void apply_move(Board* board, const Position* from, const Position* to);

#endif
