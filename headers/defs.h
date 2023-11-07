#ifndef DEFS_H
#define DEFS_H
#include "initial_board.h"
#define BOARD_SIZE 8
#define DEPTH 2
#define HUMAN_CONTROL true
#define PAWN 0
#define KNIGHT 1
#define BISHOP 2
#define ROOK 3
#define QUEEN 4
#define KING 5
#define PAWN_M2 6
#define U_KING 7
#define U_ROOK 8
#define NUM_PIECE_TYPES 6
#define BLACK 1
#define WHITE 0
#define PROMOTION_ROW_HUMAN 1
#define COMPUTER_COLOR BLACK
#define HUMAN_COLOR !COMPUTER_COLOR
#define NUM_HIDDEN_NEURONS 64
#endif