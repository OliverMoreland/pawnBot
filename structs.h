#ifndef STRUCTS_H
#define STRUCTS_H
#include "defs.h"
#include "stdbool.h"
typedef int board[BOARD_SIZE][BOARD_SIZE];

typedef struct boardPosition
{
     bool blackMove;
     int eval;
     board position;
     struct boardPosition *children;
} boardPosition;

typedef struct boardLinkedList
{
     board current;
     struct boardLinkedList *next;
} boardLinkedList;

typedef struct piece {
     int x;
     int y;
     int type;
} piece;


#endif