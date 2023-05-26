#ifndef BOARD_PIECE_H
#define BOARD_PIECE_H
#include "structs.h"

void print_board(board toPrint);
void print_board_copy(board toPrint);
piece find_piece(board position, int pieceToFind);
bool in_check(board position, piece king);
boardLinkedList *move_piece(board newBoard, boardLinkedList *head, piece king);
bool is_legal_move(board position, board move, bool playerIsBlack);
bool is_same_board(board b1, board b2);
#endif