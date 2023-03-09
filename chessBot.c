#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define BOARD_SIZE 8

/* For final
#define _INITIAL_BOARD_STATE          \
     {                               \
          { 7, 3, 5, 9,11, 5, 3, 7}, \
          { 1, 1, 1, 1, 1, 1, 1, 1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          { 0, 0, 0, 0, 0, 0, 0, 0}, \
          { 6, 2, 4, 8,10, 4, 2, 6}  \
     }
*/

#define INITIAL_BOARD_STATE          \
     {                               \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,2,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}  \
     }

typedef enum { false, true } bool;

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
12 = PawnM2 |  13 = PawnM2

PawnM2 is a pawn that has just moved two spaces forward, to allow for en passante

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
     struct boardPosition *children;
} boardPosition;



typedef struct boardLinkedList
{
     board current;
     struct boardLinkedList *next;
} boardLinkedList;

void freeBLL(boardLinkedList *toFree){
     if((*toFree).next != 0)
     {
          freeBLL((*toFree).next);
     }
     free(toFree);
     return;
}

boardLinkedList* appendToBLL(board toAppend,boardLinkedList *head)
{
     
     memcpy( (*head).current,toAppend, sizeof( (*head).current ) );
     (*head).next = (boardLinkedList *)malloc(sizeof(boardLinkedList));
     head = (*head).next;
     (*head).next = 0;

     return head;
}
void printBLL(boardLinkedList root){
     if(root.next == 0){
          return;
     }
     printf("\n\n\nBoard\n\n");
     for (int y = 0; y < BOARD_SIZE; y++)
     {
          printf("|");

          for (int x = 0; x < BOARD_SIZE; x++)
          {
               printf(" %2d |",root.current[y][x]);
          }
          printf("\n");
     }
     printBLL(*(root.next));


}



boardLinkedList *getPossibleMovesPawn(board position, int x, int y, boardLinkedList *head, bool playerIsWhite)
{
     int direction = playerIsWhite ? -1 : 1;
     board newPosition;
     if(position[y+direction][x] == -1)
     {
          //Standard Movement
          memcpy(newPosition,position,sizeof(newPosition));
          newPosition[y+direction][x] = 1-playerIsWhite;
          newPosition[y][x] = -1;
          head = appendToBLL(newPosition, head);

          //Double Movement
          if(y == 1+5*playerIsWhite &&  position[y+direction*2][x]  == -1)
          {
               memcpy(newPosition,position,sizeof(newPosition));
               newPosition[y+direction*2][x] = 13-playerIsWhite;
               newPosition[y][x] = -1;
               head = appendToBLL(newPosition, head);
          }
     }
     //Capture
     if(position[y+direction][x-1] >= 0 && !((position[y+direction][x-1] & 1) - playerIsWhite))
     {
          memcpy(newPosition,position,sizeof(newPosition));
          newPosition[y+direction][x-1] = 1-playerIsWhite;
          newPosition[y][x] = -1;
          head = appendToBLL(newPosition, head);
     
     }
     if(position[y+direction][x+1] >= 0 && !((position[y+direction][x+1] & 1) - playerIsWhite))
     {
          memcpy(newPosition,position,sizeof(newPosition));
          newPosition[y+direction][x+1] = 1-playerIsWhite;
          newPosition[y][x] = -1;
          head = appendToBLL(newPosition, head);
     
     }
     //En Passante
     if(position[y][x-1] == 12+playerIsWhite && position[y+direction][x-1] == -1 && !((position[y][x-1] & 1) - playerIsWhite))
     {
          memcpy(newPosition,position,sizeof(newPosition));
          newPosition[y+direction][x-1] = position[y][x];
          newPosition[y][x] = -1;
          newPosition[y][x-1] = -1;

          head = appendToBLL(newPosition, head);
     
     }
     if(position[y][x+1] == 12+playerIsWhite && position[y+direction][x+1] == -1 && !((position[y][x+1] & 1) - playerIsWhite))
     {
          memcpy(newPosition,position,sizeof(newPosition));
          newPosition[y+direction][x+1] = position[y][x];
          newPosition[y][x] = -1;
          newPosition[y][x+1] = -1;

          head = appendToBLL(newPosition, head);
     
     }


     return head;

}    
boardLinkedList *getPossibleMovesKnight(board position, int x, int y, boardLinkedList *head, bool playerIsWhite)
{
     board newPosition;

     for(int i = 0;i<8;i++){
          /*
           0 1
          2   3

          6   7
           4 5 
          
          This is probably needlessly complicated
          */
          int xToCheck = x + (((i / 2) & 1)+1) * ((i & 1)*2 - 1);    

          if(xToCheck > BOARD_SIZE || xToCheck < 0){
               continue;
          }
          int yToCheck = y + (!((i / 2) & 1)+1) * (((i/4)*2 - 1));
          if(yToCheck>BOARD_SIZE || yToCheck < 0)
          {
               continue;
          }
          if(((position[yToCheck][xToCheck] & 1) - playerIsWhite))
          {
               continue;
          }
          memcpy(newPosition,position,sizeof(newPosition));
          newPosition[yToCheck][xToCheck] = position[y][x];
          newPosition[y][x] = -1;

          head = appendToBLL(newPosition, head);

     }
     return head;


}
boardLinkedList *getPossibleMovesBishop(board position, int x, int y, boardLinkedList *head, bool playerIsWhite)
{
     
}  
boardLinkedList *getPossibleMovesRook(board position, int x, int y, boardLinkedList *head, bool playerIsWhite)
{
     
}      
boardLinkedList *getPossibleMovesQueen(board position, int x, int y, boardLinkedList *head, bool playerIsWhite)
{
     
}      
boardLinkedList *getPossibleMovesKing(board position, int x, int y, boardLinkedList *head, bool playerIsWhite)
{
     
}   
boardLinkedList *(*functionsThatGetPossibleMoves[])(board position, int x, int y, boardLinkedList *head, bool playerIsWhite) = {
     &getPossibleMovesPawn,
     &getPossibleMovesKnight,
     &getPossibleMovesBishop,
     &getPossibleMovesRook,
     &getPossibleMovesQueen,
     &getPossibleMovesKing,
     &getPossibleMovesPawn
     };

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
               int piece = position[y][x];
               //Check if square is empty
               if (piece == -1)
               {
                    continue;
               }
               //Check if piece belongs to opposite player
               if (!((piece & 1) - playerIsWhite))
               {
                    continue;

               }
               
               int pieceType = piece / 2;
               head = functionsThatGetPossibleMoves[pieceType](position, x, y, head, playerIsWhite);
               
          }
     }
     
     return root;
}

int main()
{

     boardPosition initial = {.whiteMove = true, .position = INITIAL_BOARD_STATE, .eval = 0x7000};
     boardLinkedList *initialMoves = getPossibleMovesFromBoard(initial.position,initial.whiteMove);
     printBLL(*initialMoves);
     
     // boardLinkedList *secondMoves = getPossibleMovesFromBoard(initialMoves->next->current,false);
     
     // printf("\n\nSecond Move from initialMoves next\n\n");
     // boardLinkedList *thirdMoves = getPossibleMovesFromBoard(secondMoves->next->current,true);
     // printBLL(*secondMoves);
     // freeBLL(secondMoves);
     // printf("\n\nThird Move from secondMoves next\n\n");
     // printBLL(*thirdMoves);
     // freeBLL(thirdMoves);

     

     freeBLL(initialMoves);
     // printf("%i",sizeof(boardPosition));
}
