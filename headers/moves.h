#ifndef MOVES_H
#define MOVES_H
#include "structs.h"
boardLinkedList *getPossibleMovesFromBoard(board position, bool playerIsBlack);
boardLinkedList *getPossibleMovesFromPiece(board position, int x, int y, boardLinkedList *head, bool playerIsBlack, piece king);

#endif