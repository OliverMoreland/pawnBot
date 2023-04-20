

#pragma region // Includes, defines and globals

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>      /* wint_t */
#include <locale.h>
#define ARR_SIZE(arr) ( sizeof((arr)) / sizeof((arr[0])) )
#define BOARD_SIZE 8
#define COMPUTER_IS_WHITE false
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
          {17, 3, 5, 9,15, 5, 3, 17}, \
          { 1, 1, 1,-1, 1, 1, 1, 1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1, 4,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          { 0, 0, 0, 0, 0, 0, 0, 0}, \
          {16, 2, 4, 8,14, 4, 2, 16}  \
     }*/
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
#define NUM_PIECE_TYPES 6
const char pieces_chars[6] = {'p','k','b','r','Q','K'};
const char x_chars[8] = {'a','b','c','d','e','f','g','h'};
const char y_chars[8] = {'8','7','6','5','4','3','2','1'};

//Flipped for different terminals: 
const int pieceChars[19] = {0x0020,0x265F,0x2659, 0x265E,0x2658, 0x265D,0x2657, 0x265C,0x2656, 0x265B,0x2655, 0x265A,0x2654, 0x265F,0x2659, 0x265A,0x2654, 0x265C,0x2656};
//const int pieceChars[19] = {0x0020,0x2659,0x265F, 0x2658,0x265E, 0x2657, 0x265D, 0x2656,0x265C, 0x2655,0x265B, 0x2654,0x265A, 0x2659,0x265F, 0x2654,0x265A, 0x2656,0x265C};

const short pointValues[6] = {1, 3, 3, 5, 9};
#pragma endregion

#pragma region // Board, and BLL and Position Processing




typedef int board[BOARD_SIZE][BOARD_SIZE];

void printBoard (board toPrint){
     printf("   a  b  c  d  e  f  g  h \n");
     for (int y = 0; y < BOARD_SIZE; y++)
     {
          printf("%d ",8-y);
          for (int x = 0; x < BOARD_SIZE; x++)
          {
               //printf("%d ",toPrint[y][x]+1);

               if(y&1 ^ x&1){
                    printf("\x1b[40m %lc ", (wint_t)pieceChars[toPrint[y][x]+1]);
               }else{
                    printf("\x1b[41m %lc ", (wint_t)pieceChars[toPrint[y][x]+1]);
               }
          }
          printf("\x1b[40m %d\n",8-y);
     }
     printf("   a  b  c  d  e  f  g  h \n");

}

typedef struct boardPosition
{
     bool blackMove;
     short eval;
     board position;
     struct boardPosition *children;
} boardPosition;

typedef struct boardLinkedList
{
     board current;
     struct boardLinkedList *next;
} boardLinkedList;

int BLL_length(boardLinkedList *BLL){
     if(BLL->next == 0){
          return 0;
     }
     return BLL_length(BLL->next)+1;
}

void BLL_item(boardLinkedList *BLL,int index, board out){
     if(index == 0){
          memcpy(out,BLL->current,sizeof(board));
          return;
     }
     BLL_item(BLL->next,index-1,out);
}

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

void printBLL(boardLinkedList *root){
     if(root->next == 0){
          printf("\nDONE\n");
          return;
     }
     printf("\n\n\nBoard\n\n");
     printBoard(root->current);
     printBLL(root->next);


}

typedef struct piece {
     int x;
     int y;
     int type;
} piece;

piece findPiece(board position, int pieceToFind){
     piece ret;
     for(int i = 0; i < BOARD_SIZE; i++){
          for(int j = 0; j < BOARD_SIZE; j++){
               if(position[i][j] == pieceToFind){
                    ret.x = j;
                    ret.y = i;
                    ret.type = position[i][j];
                    return ret;
               }
          }
     }
     ret.type = -1;
     return ret;
}

bool inCheck(board position,piece king){
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
          (GET_TYPE(position[yToCheck][xToCheck]) != KNIGHT)
          ||
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
               int yToCheck = king.y + i*yDirection;
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
                    ||
                    (
                         GET_TYPE(pieceToCheck) != BISHOP 
                         &&
                         GET_TYPE(pieceToCheck) != QUEEN
                         &&
                         (
                              i == 1
                              &&
                              (
                                   GET_TYPE(pieceToCheck) != KING
                                   ||
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
                    ||
                    (
                         GET_TYPE(pieceToCheck) != ROOK 
                         &&
                         GET_TYPE(pieceToCheck) != QUEEN
                    )
                    ||
                    (
                         i == 1
                         &&
                         (
                              GET_TYPE(pieceToCheck) != KING
                              ||
                              GET_TYPE(pieceToCheck) != U_KING
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

boardLinkedList* movePiece(board newBoard,boardLinkedList *head, piece king){
     if(inCheck(newBoard,king))
          return head;
     return appendToBLL(newBoard, head);
}


#pragma endregion

#pragma region // Possible Move Processing
// This is a group of functions that define how the pieces move (promotion and en passante not yet implemented)





boardLinkedList *getPossibleMovesPawn(board position, int x, int y, boardLinkedList *head, bool playerIsBlack, piece king)
{
     int direction = FORWARD(position[y][x]);
     board newPosition;
     memcpy(newPosition,position,sizeof(newPosition));

     if(IS_EMPTY(position[y+direction][x]))
     {
          //Standard Movement
          newPosition[y+direction][x] = PAWN*2+playerIsBlack;
          newPosition[y][x] = -1;
          head = movePiece(newPosition,head,king);
          memcpy(newPosition,position,sizeof(newPosition));
          //Double Movement
          if(y == 6-5*playerIsBlack &&  IS_EMPTY(position[y+direction*2][x]))
          {
               newPosition[y+direction*2][x] = PAWN_M2*2+playerIsBlack;
               newPosition[y][x] = -1;
               head = movePiece(newPosition,head,king);
               memcpy(newPosition,position,sizeof(newPosition));

          }
     }
     //Capture
     if(!IS_EMPTY(position[y+direction][x-1]) && (IS_BLACK(position[y+direction][x-1]) ^ playerIsBlack))
     {
          newPosition[y+direction][x-1] = PAWN*2+playerIsBlack;
          newPosition[y][x] = -1;
          head = movePiece(newPosition,head,king);
          memcpy(newPosition,position,sizeof(newPosition));     
     }
     if(!IS_EMPTY(position[y+direction][x+1]) && (IS_BLACK(position[y+direction][x+1]) ^ playerIsBlack))
     {
          newPosition[y+direction][x+1] = PAWN*2+playerIsBlack;
          newPosition[y][x] = -1;
          head = movePiece(newPosition,head,king);
          memcpy(newPosition,position,sizeof(newPosition));

     }
     //En Passante
     if(GET_TYPE(position[y][x-1]) == PAWN_M2 && IS_EMPTY(position[y+direction][x-1]) && (IS_BLACK(position[y][x-1]) ^ playerIsBlack))
     {
          newPosition[y+direction][x-1] = PAWN*2+playerIsBlack;
          newPosition[y][x] = -1;
          newPosition[y][x-1] = -1;
          head = movePiece(newPosition,head,king);
          memcpy(newPosition,position,sizeof(newPosition));

     }
     if(GET_TYPE(position[y][x+1]) == PAWN_M2 && IS_EMPTY(position[y+direction][x+1]) && (IS_BLACK(position[y][x+1]) ^ playerIsBlack))
     {
          newPosition[y+direction][x+1] = PAWN*2+playerIsBlack;
          newPosition[y][x] = -1;
          newPosition[y][x+1] = -1;
          head = movePiece(newPosition,head,king);
          memcpy(newPosition,position,sizeof(newPosition));     
     }


     return head;

}    
boardLinkedList *getPossibleMovesKnight(board position, int x, int y, boardLinkedList *head, bool playerIsBlack, piece king)
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
          playerIsBlack = 1 when player is white
          Four combinations

          black piece     white player
          1 1 Good - Can Capture
          1 0 Bad  - Same Color
          0 1 Bad  - Same Color
          0 0 Good - Can Capture
          

          */
          if(!(IS_BLACK(position[yToCheck][xToCheck]) ^ playerIsBlack))
          {
               continue;
          }
          memcpy(newPosition,position,sizeof(newPosition));
          newPosition[yToCheck][xToCheck] = position[y][x];
          newPosition[y][x] = -1;
          
          head = movePiece(newPosition,head,king);

     }
     return head;


}
boardLinkedList *getPossibleMovesBishop(board position, int x, int y, boardLinkedList *head, bool playerIsBlack, piece king)
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
                    playerIsBlack = 1 when player is white
                    Four combinations

                    black piece     white player
                    1 1 Good - Can Capture
                    1 0 Bad  - Same Color
                    0 1 Bad  - Same Color
                    0 0 Good - Can Capture
               */
               if(!(IS_BLACK(position[yToCheck][xToCheck]) ^ playerIsBlack))
               {
                    break;
               }
               memcpy(newPosition,position,sizeof(newPosition));
               newPosition[yToCheck][xToCheck] = position[y][x];
               newPosition[y][x] = -1;
               head = movePiece(newPosition,head,king);
               if(!IS_EMPTY(position[yToCheck][xToCheck]))
               {
                    break;
               }
          }

     }
     return head;
}  
boardLinkedList *getPossibleMovesRook(board position, int x, int y, boardLinkedList *head, bool playerIsBlack, piece king)
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
                    playerIsBlack = 1 when player is white
                    Four combinations

                    black piece     white player
                    1 1 Good - Can Capture
                    1 0 Bad  - Same Color
                    0 1 Bad  - Same Color
                    0 0 Good - Can Capture
               */
               if(!(IS_BLACK(position[yToCheck][xToCheck]) ^ playerIsBlack))
               {
                    break;
               }
               memcpy(newPosition,position,sizeof(newPosition));
               newPosition[yToCheck][xToCheck] = 7-playerIsBlack;
               newPosition[y][x] = -1;
               head = movePiece(newPosition,head,king);
               //printf("%i, %i\n",xToCheck,yToCheck);
               if(!IS_EMPTY(position[yToCheck][xToCheck]))
               {
                    break;
               }
          }

     }
     
     return head; 
}      
boardLinkedList *getPossibleMovesQueen(board position, int x, int y, boardLinkedList *head, bool playerIsBlack, piece king)
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
                    playerIsBlack = 1 when player is white
                    Four combinations

                    black piece     white player
                    1 1 Good - Can Capture
                    1 0 Bad  - Same Color
                    0 1 Bad  - Same Color
                    0 0 Good - Can Capture
               */
               if(!(IS_BLACK(position[yToCheck][xToCheck]) ^ playerIsBlack))
               {
                    break;
               }
               memcpy(newPosition,position,sizeof(newPosition));
               newPosition[yToCheck][xToCheck] = QUEEN*2+playerIsBlack;
               newPosition[y][x] = -1;
               head = movePiece(newPosition,head,king);
               if(!IS_EMPTY(position[yToCheck][xToCheck]))
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
                    playerIsBlack = 1 when player is white
                    Four combinations

                    black piece     white player
                    1 1 Good - Can Capture
                    1 0 Bad  - Same Color
                    0 1 Bad  - Same Color
                    0 0 Good - Can Capture
               */
               if(!(IS_BLACK(position[yToCheck][xToCheck]) ^ playerIsBlack))
               {
                    break;
               }
               memcpy(newPosition,position,sizeof(newPosition));
               newPosition[yToCheck][xToCheck] = QUEEN*2+playerIsBlack;
               newPosition[y][x] = -1;
               head = movePiece(newPosition,head,king);
               //printf("%i, %i\n",xToCheck,yToCheck);
               if(!IS_EMPTY(position[yToCheck][xToCheck]))
               {
                    break;
               }
          }

     }
 
     
     return head;
}      
boardLinkedList *getPossibleMovesKing(board position, int x, int y, boardLinkedList *head, bool playerIsBlack, piece king)
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
          if(!(IS_BLACK(position[yToCheck][xToCheck]) ^ playerIsBlack))
          {
               continue;
          }
          memcpy(newPosition,position,sizeof(newPosition));
          newPosition[yToCheck][xToCheck] = position[y][x];
          newPosition[y][x] = -1;
          head = movePiece(newPosition,head,king);

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

          if(!(IS_BLACK(position[yToCheck][xToCheck]) ^ playerIsBlack))
          {
               continue;
          }

          memcpy(newPosition,position,sizeof(newPosition));
          newPosition[yToCheck][xToCheck] = position[y][x];
          newPosition[y][x] = -1;
          head = movePiece(newPosition,head,king);

     }
     
     
     return head;

}
boardLinkedList *getPossibleMovesUKing(board position, int x, int y, boardLinkedList *head, bool playerIsBlack, piece king)
{
     // Do Normal King Moves
     head = getPossibleMovesKing(position, x, y, head, playerIsBlack,king);

     if(!inCheck(position,king)){ // No castling out of check
          // Queenside Castle
          if(GET_TYPE(position[y][0]) == U_ROOK && IS_EMPTY(position[y][x-1]) && IS_EMPTY(position[y][x-2])){
               board newPosition;
               memcpy(newPosition,position,sizeof(newPosition));
               newPosition[y][x-1] = newPosition[y][x];
               king.x--;
               newPosition[y][x] = -1;
               if(!inCheck(newPosition,king))
               {
                    newPosition[y][x-2] = newPosition[y][x-1];
                    king.x--;
                    newPosition[y][x-1] = -1;
                    if(!inCheck(newPosition,king)){
                         newPosition[y][x-1] = newPosition[y][0];
                         newPosition[y][0] = -1;
                         head = movePiece(newPosition,head,king);
                    }
               }
               
          }
          // Kingside Castle
          if(GET_TYPE(position[y][BOARD_SIZE-1]) == U_ROOK && IS_EMPTY(position[y][x+1]) && IS_EMPTY(position[y][x+2])){
               board newPosition;
               memcpy(newPosition,position,sizeof(newPosition));
               newPosition[y][x+1] = newPosition[y][x];
               king.x++;
               newPosition[y][x] = -1;
               if(!inCheck(newPosition,king))
               {
                    newPosition[y][x+2] = newPosition[y][x+1];
                    king.x++;
                    newPosition[y][x+1] = -1;
                    if(!inCheck(newPosition,king)){
                         newPosition[y][x+1] = newPosition[y][0];
                         newPosition[y][BOARD_SIZE-1] = -1;
                         head = movePiece(newPosition,head,king);
                    }
               }
               
          }
    
     }
     return head;

}




boardLinkedList *(*functionsThatGetPossibleMoves[])(board position, int x, int y, boardLinkedList *head, bool playerIsBlack, piece king) = {
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

boardLinkedList *getPossibleMovesFromBoard(board position, bool playerIsBlack)
{
     boardLinkedList *root;
     boardLinkedList *head;
     root = (boardLinkedList *)malloc(sizeof(boardLinkedList));
     head = root;
     piece king = findPiece(position, KING*2+playerIsBlack);
     if(IS_EMPTY(king.type)){
          king = findPiece(position, U_KING*2+playerIsBlack);
     }
     for (int y = 0; y < BOARD_SIZE; y++)
     {
          for (int x = 0; x < BOARD_SIZE; x++)
          {
               int piece = position[y][x];
               //Check if square is empty
               if (IS_EMPTY(piece))
               {
                    continue;
               }
               //Check if piece belongs to opposite player
               if (IS_BLACK(piece) ^ playerIsBlack)
               {
                    continue;

               }

               head = functionsThatGetPossibleMoves[GET_TYPE(piece)](position, x, y, head, playerIsBlack,king);
               
          }
     }
     if(head == root){
          printf("getPossibleMoves returned null");
     }

     return root;
}
#pragma endregion

#pragma region // Move Choosing

void computer_move(board current){
     boardPosition state = {.blackMove = !COMPUTER_IS_WHITE, .eval = 0x7000};
     memcpy(state.position,current,sizeof(board));
     boardLinkedList *possibleMoves = getPossibleMovesFromBoard(state.position,state.blackMove);
     if(possibleMoves->next == 0){
          printf("0 comp moves generated");
          free(possibleMoves);
          return;
     }
     int index = rand() % BLL_length(possibleMoves);
     BLL_item(possibleMoves,index,current);

     freeBLL(possibleMoves);
}

#pragma endregion


#pragma region // Human Input Processing

piece read_piece(){
     char p, x, y;
     piece ret;
     printf("Make your move: ");
     scanf("\n%c%c%c",&p,&x,&y);
     printf("Received p: %c, x: %c, y: %c\n",p,x,y);
     for(int i = 0;i<NUM_PIECE_TYPES;i++){
          if(pieces_chars[i] == p){
               ret.type = i;
          }
     }
     for(int i = 0;i<BOARD_SIZE;i++){
          if(x_chars[i] == x){
               ret.x = i;
          }
     }
     for(int i = 0;i<BOARD_SIZE;i++){
          if(y_chars[i] == y){
               ret.y = i;
          }
     }
     printf("Moved to: %d, x: %d, y: %d\n",ret.type,ret.x,ret.y);
     
     return ret;
}


void human_move(board current){
     boardPosition state = {.blackMove = COMPUTER_IS_WHITE, .eval = 0x7000};
     memcpy(state.position,current,sizeof(board));
     boardLinkedList *possibleMoves = getPossibleMovesFromBoard(state.position,state.blackMove);
     while(true){ // repeat so players can retry if they make an illegal move
          boardLinkedList *head = possibleMoves;
          piece to_move = read_piece();
          if(to_move.x < 0 || to_move.x >= BOARD_SIZE || to_move.y < 0 || to_move.y >= 8 || to_move.type < 0 || to_move.type >= NUM_PIECE_TYPES){
               printf("Error: illegal move - reading\n");
               continue;
          }
          while(head->next != 0){
               int piece_to_check = head->current[to_move.y][to_move.x];
               if(
                    IS_BLACK(piece_to_check) == COMPUTER_IS_WHITE
                    &&
                    (
                         GET_TYPE(piece_to_check) == to_move.type 
                         || 
                         (GET_TYPE(piece_to_check) == PAWN && to_move.type == PAWN_M2) 
                         || 
                         (GET_TYPE(piece_to_check) == PAWN_M2 && to_move.type == PAWN) 
                         || 
                         (GET_TYPE(piece_to_check) == ROOK && to_move.type == U_ROOK) 
                         || 
                         (GET_TYPE(piece_to_check) == KING && to_move.type == U_KING)
                    )
               )
               {
                    memcpy(current,head->current,sizeof(board));
                    freeBLL(possibleMoves);
                    return;
               }else{
                    printf("%d %d %d\n",piece_to_check,GET_TYPE(piece_to_check), to_move.type);
               }
               head = head->next;
          }
          if(head->next == 0){
               printf("Error: illegal move - generating\n");
          }
     }
     freeBLL(possibleMoves);
}

#pragma endregion


int main()
{
     setlocale(LC_ALL, "");     
     srand(time(NULL));
     //printBLL(initialMoves);
     board current = INITIAL_BOARD_STATE;
     if(!COMPUTER_IS_WHITE){
          printBoard(current);
          human_move(current);
          printBoard(current);
     }
     while(true){
          computer_move(current);
          printBoard(current);
          human_move(current);
          printBoard(current);
          sleep(0.1);
     }
     //printBoard(current);
     //printf("\n%d\n",inCheck(current,findPiece(current,U_KING*2+1)));

}