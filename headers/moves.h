#ifndef MOVES_H
#define MOVES_H
#include "structs.h"
boardLinkedList *get_possible_moves_from_board(board position, bool playerIsBlack);
coordLL *get_possible_moves_from_piece(board position, int x, int y, bool playerIsBlack);
#endif