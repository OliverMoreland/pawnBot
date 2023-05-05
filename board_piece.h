#ifndef BOARD_PIECE_H
#define BOARD_PIECE_H
#include "structs.h"

void printBoard (board toPrint);
void printBoardCopy (board toPrint);
piece findPiece(board position, int pieceToFind);
bool inCheck(board position,piece king);
boardLinkedList* movePiece(board newBoard,boardLinkedList *head, piece king);

#endif