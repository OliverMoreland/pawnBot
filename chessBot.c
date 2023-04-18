#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define BOARD_SIZE 8
// For final
#define INITIAL_BOARD_STATE          \
     {                               \
          { 17, 3, 5, 9,15, 5, 3, 17}, \
          { 1, 1, 1, 1, 1, 1, 1, 1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          { 0, 0, 0, 0, 0, 0, 0, 0}, \
          { 16, 2, 4, 8,14, 4, 2, 16}  \
     }

/* 
#define INITIAL_BOARD_STATE          \
     {                               \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1, 6,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}  \
     }
*/
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
14 = UKing  |  15 = UKing
16 = URook  |  17 = URook

So GET_TYPE(piece) returns:
0 pawn
1 knight
2 Bishop
3 Rook
4 Queen
5 King
6 PawnM2
7 UKing
8 URook


UKing is an unmoved king (that can castle)
URook is an unmoved rook
PawnM2 is a pawn that has just moved two spaces forward, to allow for en passante

Black is up,
White is down
Boards are [UP->DOWN = 0 - 7][LEFT-RIGHT = 0-7]

*/
#define FORWARD(piece) ((piece & 1)*2-1)
#define IS_BLACK(piece) (piece & 1)
#define GET_TYPE(piece) (piece >> 1)
#define IS_EMPTY(piece) (piece == -1)
#define PAWN 0
#define KNIGHT 1
#define BISHOP 2
#define ROOK 3
#define QUEEN 4
#define KING 5
#define PAWN_M2 6
#define U_KING 7
#define U_ROOK 8

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

typedef struct piece {
     int x;
     int y;
     int type;
} location;

void freeBLL(boardLinkedList *toFree){
     if(toFree->next != 0)
     {
          freeBLL(toFree->next);
     }
     free(toFree);
     return;
}

boardLinkedList* appendToBLL(board toAppend,boardLinkedList *head)
{
     
     memcpy( head->current,toAppend, sizeof( head->current ) );
     head->next = (boardLinkedList *)malloc(sizeof(boardLinkedList));
     head = head->next;
     head->next = 0;

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


location findPiece(board position, bool pieceToFind){
     for(int i = 0; i < BOARD_SIZE; i++){
          for(int j = 0; j < BOARD_SIZE; j++){
               if(position[i][j] == pieceToFind){
                    location ret;
                    ret.x = j;
                    ret.y = i;
                    ret.type = position[i][j];
                    return ret;
               }
          }
     }
     
}

bool inCheck(board position,location king){
     // Check Knights
     for(int i = 0;i<8;i++){
          /*
           0 1
          2   3

          6   7
           4 5 
          
          This is probably needlessly complicated
          */
          int xToCheck = king.x + (((i >> 1) & 1)+1) * (((i & 1)<<1) - 1);    

          if(xToCheck >= BOARD_SIZE || xToCheck < 0){
               continue;
          }
          int yToCheck = king.y + (!((i >> 1) & 1)+1) * (((i>>2)<<1) - 1);

          if(yToCheck>=BOARD_SIZE || yToCheck < 0)
          {
               continue;
          }
          //First conditional checks if it is a knight.
          //Second conditional checks if the two the piece and the king are the same color. 
          if(
          (GET_TYPE(position[yToCheck][xToCheck]) == KNIGHT)
          &&
          !(IS_BLACK(position[yToCheck][xToCheck]) ^ IS_BLACK(king.type))
          )
          {
               continue;
          }
          return true;
     }

     // I partially stole this code from the queen move checking 
     // What these loops are doing: first loop is direction, second is distance from piece. Inner loop breaks when it encounters a piece or goes off the board.
     // Check Bishop + Queen + King
     for(int j = 0;j<4;j++){
          int xDirection = (((j&2)>>1)*2-1);
          int yDirection = ((j&1)*2-1);
          for(int i = 1;i<8;i++){
               int xToCheck = king.x + i*xDirection;
               if(xToCheck >= BOARD_SIZE || xToCheck < 0){
                    break;
               }
               int yToCheck = king.x + i*yDirection;
               if(yToCheck>=BOARD_SIZE || yToCheck < 0)
               {
                    break;
               }
               int pieceToCheck = position[yToCheck][xToCheck];
               if(IS_EMPTY(pieceToCheck)){
                    continue;
               }
               if(

                    !(IS_BLACK(pieceToCheck) ^ IS_BLACK(king.type))
                    &&
                    (
                         GET_TYPE(pieceToCheck) != BISHOP 
                         && 
                         GET_TYPE(pieceToCheck) != QUEEN
                         &&
                         (
                              i != 1
                              ||
                              (
                                   GET_TYPE(pieceToCheck) != KING
                                   &&
                                   GET_TYPE(pieceToCheck) != U_KING
                              )
                         )
                    )
               ){
                    break;
               }
               return true;
          }

     }
     // Check Rook + Queen + King
     for(int j = 0;j<4;j++){
          
          int xDirection = (j & 1) * (j-2);
          int yDirection = (!(j & 1)) * (j-1);
          for(int i = 1;i<8;i++){
               int xToCheck = king.x + i*xDirection;
               if(xToCheck >= BOARD_SIZE || xToCheck < 0){
                    break;
               }
               int yToCheck = king.x + i*yDirection;
               if(yToCheck>=BOARD_SIZE || yToCheck < 0)
               {
                    break;
               }
               int pieceToCheck = position[yToCheck][xToCheck];
               if(IS_EMPTY(pieceToCheck)){
                    continue;
               }
               if(

                    !(IS_BLACK(pieceToCheck) ^ IS_BLACK(king.type))
                    &&
                    (
                         GET_TYPE(pieceToCheck) != ROOK 
                         && 
                         GET_TYPE(pieceToCheck) != QUEEN
                         &&
                         (
                              i != 1
                              ||
                              (
                                   GET_TYPE(pieceToCheck) != KING
                                   &&
                                   GET_TYPE(pieceToCheck) != U_KING
                              )
                         )
                    )
               ){
                    break;
               }
               return true;
          }
     }
     
     // Check Pawn
     if(
          (
               GET_TYPE(position[king.y+FORWARD(king.type)][king.x-1]) == PAWN
               &&
               IS_BLACK(position[king.y+FORWARD(king.type)][king.x-1]) ^ IS_BLACK(king.type)
          )
          ||
          (
               GET_TYPE(position[king.y+FORWARD(king.type)][king.x+1]) == PAWN
               &&
               IS_BLACK(position[king.y+FORWARD(king.type)][king.x+1]) ^ IS_BLACK(king.type)
          )
     ){
          return true;
     }
     return false;
     
}

/*
     This is a group of functions that define how the pieces move (promotion and castling not yet implemented)
*/





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
     if(position[y+direction][x-1] >= 0 && !((position[y+direction][x-1] & 1) ^ playerIsWhite))
     {
          memcpy(newPosition,position,sizeof(newPosition));
          newPosition[y+direction][x-1] = 1-playerIsWhite;
          newPosition[y][x] = -1;
          head = appendToBLL(newPosition, head);
     
     }
     if(position[y+direction][x+1] >= 0 && !((position[y+direction][x+1] & 1) ^ playerIsWhite))
     {
          memcpy(newPosition,position,sizeof(newPosition));
          newPosition[y+direction][x+1] = 1-playerIsWhite;
          newPosition[y][x] = -1;
          head = appendToBLL(newPosition, head);
     
     }
     //En Passante
     if(position[y][x-1] == 12+playerIsWhite && position[y+direction][x-1] == -1 && !((position[y][x-1] & 1) ^ playerIsWhite))
     {
          memcpy(newPosition,position,sizeof(newPosition));
          newPosition[y+direction][x-1] = position[y][x];
          newPosition[y][x] = -1;
          newPosition[y][x-1] = -1;

          head = appendToBLL(newPosition, head);
     
     }
     if(position[y][x+1] == 12+playerIsWhite && position[y+direction][x+1] == -1 && !((position[y][x+1] & 1) ^ playerIsWhite))
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
          int xToCheck = x + (((i >> 1) & 1)+1) * (((i & 1)<<1) - 1);    

          if(xToCheck >= BOARD_SIZE || xToCheck < 0){
               continue;
          }
          int yToCheck = y + (!((i >> 1) & 1)+1) * (((i>>2)<<1) - 1);
          if(yToCheck>=BOARD_SIZE || yToCheck < 0)
          {
               continue;
          }
          /*
          How this works:
          (position[yToCheck][xToCheck] & 1) returns 1 if piece is black
          playerIsWhite = 1 when player is white
          Four combinations

          black piece     white player
          1 1 Good - Can Capture
          1 0 Bad  - Same Color
          0 1 Bad  - Same Color
          0 0 Good - Can Capture
          

          */
          if((position[yToCheck][xToCheck] & 1) ^ playerIsWhite)
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
     board newPosition;
     int xToCheck;
     int yToCheck;
     int xDirection;
     int yDirection;
     // What these loops are doing: first loop is direction, second is distance from piece. Inner loop breaks when it encounters a piece or goes off the board.
     for(int j = 0;j<4;j++){
          xDirection = (((j&2)>>1)*2-1);
          yDirection = ((j&1)*2-1);
          for(int i = 1;i<8;i++){
               xToCheck = x + i*xDirection;
               if(xToCheck >= BOARD_SIZE || xToCheck < 0){
                    break;
               }
               yToCheck = y + i*yDirection;
               if(yToCheck>=BOARD_SIZE || yToCheck < 0)
               {
                    break;
               }
               /*
                    How this works (Same as in other functions):
                    (position[yToCheck][xToCheck] & 1) returns 1 if piece is black
                    playerIsWhite = 1 when player is white
                    Four combinations

                    black piece     white player
                    1 1 Good - Can Capture
                    1 0 Bad  - Same Color
                    0 1 Bad  - Same Color
                    0 0 Good - Can Capture
               */
               if((position[yToCheck][xToCheck] & 1) ^ playerIsWhite)
               {
                    break;
               }
               memcpy(newPosition,position,sizeof(newPosition));
               newPosition[yToCheck][xToCheck] = position[y][x];
               newPosition[y][x] = -1;
               head = appendToBLL(newPosition, head);
               if(position[yToCheck][xToCheck] != -1)
               {
                    break;
               }
          }

     }
     return head;
}  
boardLinkedList *getPossibleMovesRook(board position, int x, int y, boardLinkedList *head, bool playerIsWhite)
{
         board newPosition;
     int xToCheck;
     int yToCheck;
     int xDirection;
     int yDirection;
     // This is very similiar to Bishop code except x and y directions are different
     // What these loops are doing: first loop is direction, second is distance from piece. Inner loop breaks when it encounters a piece or goes off the board.
     for(int j = 0;j<4;j++){
          
          xDirection = (j & 1) * (j-2);
          yDirection = (!(j & 1)) * (j-1);
          for(int i = 1;i<8;i++){
               xToCheck = x + i*xDirection;
               if(xToCheck >= BOARD_SIZE || xToCheck < 0){
                    break;
               }
               yToCheck = y + i*yDirection;
               if(yToCheck>=BOARD_SIZE || yToCheck < 0)
               {
                    break;
               }
               /*
                    How this works (Same as in knight function):
                    (position[yToCheck][xToCheck] & 1) returns 1 if piece is black
                    playerIsWhite = 1 when player is white
                    Four combinations

                    black piece     white player
                    1 1 Good - Can Capture
                    1 0 Bad  - Same Color
                    0 1 Bad  - Same Color
                    0 0 Good - Can Capture
               */
               if((position[yToCheck][xToCheck] & 1) ^ playerIsWhite)
               {
                    break;
               }
               memcpy(newPosition,position,sizeof(newPosition));
               newPosition[yToCheck][xToCheck] = 7-playerIsWhite;
               newPosition[y][x] = -1;
               head = appendToBLL(newPosition, head);
               printf("%i, %i\n",xToCheck,yToCheck);
               if(position[yToCheck][xToCheck] != -1)
               {
                    break;
               }
          }

     }
     
     return head; 
}      
boardLinkedList *getPossibleMovesQueen(board position, int x, int y, boardLinkedList *head, bool playerIsWhite)
{
     board newPosition;
     int xToCheck;
     int yToCheck;
     int xDirection;
     int yDirection;
     // What these loops are doing: first loop is direction, second is distance from piece. Inner loop breaks when it encounters a piece or goes off the board.
     // Check bishop moves
     for(int j = 0;j<4;j++){
          xDirection = (((j&2)>>1)*2-1);
          yDirection = ((j&1)*2-1);
          for(int i = 1;i<8;i++){
               xToCheck = x + i*xDirection;
               if(xToCheck >= BOARD_SIZE || xToCheck < 0){
                    break;
               }
               yToCheck = y + i*yDirection;
               if(yToCheck>=BOARD_SIZE || yToCheck < 0)
               {
                    break;
               }
               /*
                    How this works (Same as in knight function):
                    (position[yToCheck][xToCheck] & 1) returns 1 if piece is black
                    playerIsWhite = 1 when player is white
                    Four combinations

                    black piece     white player
                    1 1 Good - Can Capture
                    1 0 Bad  - Same Color
                    0 1 Bad  - Same Color
                    0 0 Good - Can Capture
               */
               if((position[yToCheck][xToCheck] & 1) ^ playerIsWhite)
               {
                    break;
               }
               memcpy(newPosition,position,sizeof(newPosition));
               newPosition[yToCheck][xToCheck] = position[y][x];
               newPosition[y][x] = -1;
               head = appendToBLL(newPosition, head);
               if(position[yToCheck][xToCheck] != -1)
               {
                    break;
               }
          }

     }
     // Check rook moves
     for(int j = 0;j<4;j++){
          xDirection = (j & 1) * (j-2);
          yDirection = (!(j & 1)) * (j-1);
          for(int i = 1;i<8;i++){
               xToCheck = x + i*xDirection;
               if(xToCheck >= BOARD_SIZE || xToCheck < 0){
                    break;
               }
               yToCheck = y + i*yDirection;
               if(yToCheck>=BOARD_SIZE || yToCheck < 0)
               {
                    break;
               }
               /*
                    How this works (Same as in knight function):
                    (position[yToCheck][xToCheck] & 1) returns 1 if piece is black
                    playerIsWhite = 1 when player is white
                    Four combinations

                    black piece     white player
                    1 1 Good - Can Capture
                    1 0 Bad  - Same Color
                    0 1 Bad  - Same Color
                    0 0 Good - Can Capture
               */
               if((position[yToCheck][xToCheck] & 1) ^ playerIsWhite)
               {
                    break;
               }
               memcpy(newPosition,position,sizeof(newPosition));
               newPosition[yToCheck][xToCheck] = 7-playerIsWhite;
               newPosition[y][x] = -1;
               head = appendToBLL(newPosition, head);
               printf("%i, %i\n",xToCheck,yToCheck);
               if(position[yToCheck][xToCheck] != -1)
               {
                    break;
               }
          }

     }
 
     
     return head;
}      
boardLinkedList *getPossibleMovesKing(board position, int x, int y, boardLinkedList *head, bool playerIsWhite)
{
     board newPosition;
     int xToCheck;
     int yToCheck;
     //This is pretty much just the queen code except without the inner loops
     for(int j = 0;j<4;j++){
          xToCheck = x + (((j&2)>>1)*2-1);
          if(xToCheck >= BOARD_SIZE || xToCheck < 0){
               continue;;
          }
          yToCheck = y + ((j&1)*2-1);
          if(yToCheck>=BOARD_SIZE || yToCheck < 0)
          {
               continue;
          }
          // Works same as in other functions
          if((position[yToCheck][xToCheck] & 1) ^ playerIsWhite)
          {
               continue;
          }
          memcpy(newPosition,position,sizeof(newPosition));
          newPosition[yToCheck][xToCheck] = position[y][x];
          newPosition[y][x] = -1;
          head = appendToBLL(newPosition, head);

     }
     for(int j = 0;j<4;j++){

          xToCheck = x + (((j&2)>>1)*2-1);
          if(xToCheck >= BOARD_SIZE || xToCheck < 0){
               continue;
          }
          yToCheck = y + ((j&1)*2-1);
          if(yToCheck>=BOARD_SIZE || yToCheck < 0)
          {
               continue;
          }
          // Works same as in other functions

          if((position[yToCheck][xToCheck] & 1) ^ playerIsWhite)
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
boardLinkedList *getPossibleMovesUKing(board position, int x, int y, boardLinkedList *head, bool playerIsWhite)
{
     // Do Normal King Moves
     head = getPossibleMovesKing(position, x, y, head, playerIsWhite);
     
     // Castling TODO: Finish
     if(position[y][0] == 17-playerIsWhite){
          board newPosition;
          memcpy(newPosition,position,sizeof(newPosition));
          newPosition[y][0] = 11-playerIsWhite;
          newPosition[y][x] = -1;
          head = appendToBLL(newPosition, head);

     }

     
     return head;

}




boardLinkedList *(*functionsThatGetPossibleMoves[])(board position, int x, int y, boardLinkedList *head, bool playerIsWhite) = {
     &getPossibleMovesPawn,
     &getPossibleMovesKnight,
     &getPossibleMovesBishop,
     &getPossibleMovesRook,
     &getPossibleMovesQueen,
     &getPossibleMovesKing,
     &getPossibleMovesPawn,
     &getPossibleMovesUKing,
     &getPossibleMovesRook
     };

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
               if (!((piece & 1) ^ playerIsWhite))
               {
                    continue;

               }
               
               int pieceType = piece >> 1;
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
