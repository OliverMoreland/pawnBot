#include <stdbool.h>
#include <stdio.h>

#define BOARD_SIZE 8
#define INITIAL_BOARD_STATE                     \
     {                                          \
          {7, 3, 5, 9, 11, 5, 3, 7},            \
              {1, 1, 1, 1, 1, 1, 1, 1},         \
              {-1, -1, -1, -1, -1, -1, -1, -1}, \
              {-1, -1, -1, -1, -1, -1, -1, -1}, \
              {-1, -1, -1, -1, -1, -1, -1, -1}, \
              {-1, -1, -1, -1, -1, -1, -1, -1}, \
              {0, 0, 0, 0, 0, 0, 0, 0},         \
          {                                     \
               6, 2, 4, 8, 10, 4, 2, 6          \
          }                                     \
     }

/*Notes:
-E000 < eval < E000

Piece indexes:
-1 = Nothing
White:      |  Black:
0 = Pawn    |  1 = Pawn
2 = Knight  |  3 = Knight
4 = Bishop  |  5 = Bishop
6 = Rook    |  7 = Rook
8 = Queen   |  9 = Queen
10 = King   |  11 = King

Black is up,
White is down
Boards are [UP->DOWN = 0 - 7][LEFT-RIGHT = 0-7]

*/

const short pointValues[6] = {1, 3, 3, 5, 9};

typedef int board[BOARD_SIZE][BOARD_SIZE];

typedef struct boardPosition
{
     bool whiteMove;
     short eval;
     board position;
     boardPosition parent;
     boardPosition children;
} boardPosition;

typedef struct boardLinkedList
{
     board current;
     boardLinkedList *next;
} boardLinkedList;


void getPossibleMovesPawn(board position, int x, int y, boardLinkedList *head)
{

}

void (getPossibleMovesFromPieceFunctions*)(board position, int x, int y, boardLinkedList *head) = 

// boardLinkedList *getPossibleMovesFromPiece(board position, int x, int y, boardLinkedList *head, bool playerIsWhite)
// {
//      int piece = position[y][x];
//      int pieceType = piece / 2;
//      if(pieceType == 0)
//      {

//      }
//      if(pieceType == 1)
//      {

//      }
//      if(pieceType == 2)
//      {

//      }
//      if(pieceType == 3)
//      {

//      }
// }

boardLinkedList *getPossibleMovesFromBoard(board position, bool playerIsWhite)
{
     boardLinkedList *root;
     boardLinkedList *head;
     root = (boardLinkedList *)malloc(sizeof(boardLinkedList));
     head = root;

     for (int y = 0; y < BOARD_SIZE; y++)
     {
          for (int x = 0; x < BOARD_SIZE; x++)
          {
               //Check if square is empty
               if (position[y][x] == -1)
               {
                    continue;
               }
               //Check if piece belongs to opposite player
               if ((playerIsWhite && position[y][x] & 1 == 1) || (!playerIsWhite && position[y][x] & 1 != 1))
               {
                    continue;

               }
               head = getPossibleMovesFromPiece(position, x, y, head, playerIsWhite);
          }
     }
}

int main()
{

     boardPosition initial = {.whiteMove = true, .position = INITIAL_BOARD_STATE, .eval = 0x7000};
     // printf("%i",sizeof(boardPosition));
}
