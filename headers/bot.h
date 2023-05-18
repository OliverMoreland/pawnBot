#ifndef BOT_H
#define BOT_H
#include "structs.h"

int eval_board(board to_eval, bool computer_is_black);
int minimax_odd(int depth, board current, bool computer_is_black);
int minimax_even(int depth, board current, bool computer_is_black);
int computer_move(board current, bool computer_is_black);

#endif