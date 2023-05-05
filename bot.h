#ifndef BOT_H
#define BOT_H
#include "structs.h"

int eval_board(board to_eval);
int minimax_odd(int depth, board current);
int minimax_even(int depth, board current);
int computer_move(board current);

#endif