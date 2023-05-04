

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
#define DEPTH 3
#define COMPUTER_IS_WHITE false
// For final
/*
#define INITIAL_BOARD_STATE          \
     {                               \
          {7, 3, 5, -1, 15, -1, 3, -1}, \
          {-1, 1, 1, 1, -1, -1, -1, -1}, \
          {-1, -1, -1, -1, 1, -1, 1, 7}, \
          {-1, 1, -1, 0, -1, 1, -1, -1}, \
          {-1, -1, 14, -1, -1, 5, -1, 0}, \
          {-1, 6, -1, -1, -1, -1, -1, -1}, \
          {0, -1, 0, -1,9, -1, -1, -1} , \
          {-1, -1, -1, -1, -1, 2, -1, -1}  \
     }
//*/

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
//*/
/*
#define INITIAL_BOARD_STATE          \
     {                               \
          {17, 3, 5, 9,15,-1,-1,17}, \
          { 1, 1, 1, 1, 1, 1, 1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1, -1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1, 0, 0,-1,-1, 1}, \
          {-1,-1, 2,-1,-1, 2,-1,-1}, \
          { 0, 0, 0,-1,-1,-1, 0, 0}, \
          {16,-1, 4,8,14, 4,-1, 16}  \
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
-1000 < eval < 1000
-eval = good for human 
+eval = good for bot

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

const int pointValues[9] = {1, 3, 3, 5, 9,1000,1,1000,5};
#pragma endregion

#pragma region // Board, and BLL and Position Processing




typedef int board[BOARD_SIZE][BOARD_SIZE];

void printBoard (board toPrint){
     system("clear");
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

void printBoardCopy (board toPrint){
     //system("clear");
     printf("\n\n");
     for (int y = 0; y < BOARD_SIZE; y++)
     {
          printf("{");
          for (int x = 0; x < BOARD_SIZE; x++)
          {
               //printf("%d ",toPrint[y][x]+1);

               printf(" %d,",toPrint[y][x]);
          }
          printf(" }, \\ \n",8-y);
     }
     printf("\n\n");

}


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

boardLinkedList *HISTORY;
boardLinkedList *HISTORY_HEAD;

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

void print_BLL_item(boardLinkedList *BLL, int index){
     board toPrint;
     BLL_item(BLL,index,toPrint);
     printf("\nNumbers:\n\n");
     printBoardCopy(toPrint);
     printf("\nRendered:\n\n");
     printBoard(toPrint);
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
     
     memcpy( head->current,toAppend, sizeof(board) );
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
          (
          GET_TYPE(position[yToCheck][xToCheck]) != KNIGHT)
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

boardLinkedList* movePiece(board newBoard,boardLinkedList *head, piece king){
     if(inCheck(newBoard,king))
     {
          //printf("check ");
          return head;
     }
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
          newPosition[y][x] = -1;
          //Standard Movement
          if(y+direction == BOARD_SIZE || y+direction == 0){
               
               newPosition[y+direction][x] = KNIGHT*2+playerIsBlack;
               head = movePiece(newPosition,head,king);
               newPosition[y+direction][x] = BISHOP*2+playerIsBlack;
               head = movePiece(newPosition,head,king);
               newPosition[y+direction][x] = ROOK*2+playerIsBlack;
               head = movePiece(newPosition,head,king);
               newPosition[y+direction][x] = QUEEN*2+playerIsBlack;
               head = movePiece(newPosition,head,king);
               memcpy(newPosition,position,sizeof(newPosition));
          }else {
               newPosition[y+direction][x] = PAWN*2+playerIsBlack;
               head = movePiece(newPosition,head,king);
               memcpy(newPosition,position,sizeof(newPosition));
          }
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
     if(!IS_EMPTY(position[y+direction][x-1]) && (IS_BLACK(position[y+direction][x-1]) ^ playerIsBlack) && x-1 != 0)
     {
          newPosition[y][x] = -1;
          //Standard Movement
          if(y+direction == BOARD_SIZE || y+direction == 0){
               
               newPosition[y+direction][x-1] = KNIGHT*2+playerIsBlack;
               head = movePiece(newPosition,head,king);
               newPosition[y+direction][x-1] = BISHOP*2+playerIsBlack;
               head = movePiece(newPosition,head,king);
               newPosition[y+direction][x-1] = ROOK*2+playerIsBlack;
               head = movePiece(newPosition,head,king);
               newPosition[y+direction][x-1] = QUEEN*2+playerIsBlack;
               head = movePiece(newPosition,head,king);
               memcpy(newPosition,position,sizeof(newPosition));
          }else {
               newPosition[y+direction][x-1] = PAWN*2+playerIsBlack;
               head = movePiece(newPosition,head,king);
               memcpy(newPosition,position,sizeof(newPosition));
          }
     }
     if(!IS_EMPTY(position[y+direction][x+1]) && (IS_BLACK(position[y+direction][x+1]) ^ playerIsBlack) && x+1 != BOARD_SIZE)
     {
          newPosition[y][x] = -1;
          //Standard Movement
          if(y+direction == BOARD_SIZE || y+direction == 0){
               
               newPosition[y+direction][x+1] = KNIGHT*2+playerIsBlack;
               head = movePiece(newPosition,head,king);
               newPosition[y+direction][x+1] = BISHOP*2+playerIsBlack;
               head = movePiece(newPosition,head,king);
               newPosition[y+direction][x+1] = ROOK*2+playerIsBlack;
               head = movePiece(newPosition,head,king);
               newPosition[y+direction][x+1] = QUEEN*2+playerIsBlack;
               head = movePiece(newPosition,head,king);
               memcpy(newPosition,position,sizeof(newPosition));
          }else {
               newPosition[y+direction][x+1] = PAWN*2+playerIsBlack;
               head = movePiece(newPosition,head,king);
               memcpy(newPosition,position,sizeof(newPosition));
          }

     }
     //En Passante
     if(GET_TYPE(position[y][x-1]) == PAWN_M2 && IS_EMPTY(position[y+direction][x-1]) && (IS_BLACK(position[y][x-1]) ^ playerIsBlack) && x-1 != 0)
     {
          newPosition[y+direction][x-1] = PAWN*2+playerIsBlack;
          newPosition[y][x] = -1;
          newPosition[y][x-1] = -1;
          head = movePiece(newPosition,head,king);
          memcpy(newPosition,position,sizeof(newPosition));

     }
     if(GET_TYPE(position[y][x+1]) == PAWN_M2 && IS_EMPTY(position[y+direction][x+1]) && (IS_BLACK(position[y][x+1]) ^ playerIsBlack) && x+1 != BOARD_SIZE)
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
          if(!IS_EMPTY(position[yToCheck][xToCheck]) && !(IS_BLACK(position[yToCheck][xToCheck]) ^ playerIsBlack))
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
               if(!IS_EMPTY(position[yToCheck][xToCheck]) && !(IS_BLACK(position[yToCheck][xToCheck]) ^ playerIsBlack))
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
               if(!IS_EMPTY(position[yToCheck][xToCheck]) && !(IS_BLACK(position[yToCheck][xToCheck]) ^ playerIsBlack))
               {
                    break;
               }
               memcpy(newPosition,position,sizeof(newPosition));
               newPosition[yToCheck][xToCheck] = ROOK*2+playerIsBlack;
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
               if(!IS_EMPTY(position[yToCheck][xToCheck]) && !(IS_BLACK(position[yToCheck][xToCheck]) ^ playerIsBlack))
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
               if(!IS_EMPTY(position[yToCheck][xToCheck]) && !(IS_BLACK(position[yToCheck][xToCheck]) ^ playerIsBlack))
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
               continue;
          }
          yToCheck = y + ((j&1)*2-1);
          if(yToCheck>=BOARD_SIZE || yToCheck < 0)
          {
               continue;
          }
          // Works same as in other functions
          if(!IS_EMPTY(position[yToCheck][xToCheck]) && !(IS_BLACK(position[yToCheck][xToCheck]) ^ playerIsBlack))
          {
               continue;
          }
          king.x = xToCheck;
          king.y = yToCheck;
          memcpy(newPosition,position,sizeof(newPosition));
          newPosition[yToCheck][xToCheck] = KING*2+playerIsBlack;
          newPosition[y][x] = -1;
          head = movePiece(newPosition,head,king);

     }
     
     for(int j = 0;j<4;j++){

          xToCheck = x + (j & 1) * (j-2);
          if(xToCheck >= BOARD_SIZE || xToCheck < 0){
               continue;
          }
          yToCheck = y + (!(j & 1)) * (j-1);
          if(yToCheck>=BOARD_SIZE || yToCheck < 0)
          {
               continue;
          }
          // Works same as in other functions

          if(!IS_EMPTY(position[yToCheck][xToCheck]) && !(IS_BLACK(position[yToCheck][xToCheck]) ^ playerIsBlack))
          {
               continue;
          }
          king.x = xToCheck;
          king.y = yToCheck;
          memcpy(newPosition,position,sizeof(newPosition));
          newPosition[yToCheck][xToCheck] = KING*2+playerIsBlack;
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
               newPosition[y][x-1] = KING*2+IS_BLACK(king.type);
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
               newPosition[y][x+1] = KING*2+IS_BLACK(king.type);
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
     root->next = 0;
     head = root;
     piece pawn_to_timeout_en_pass = findPiece(position, PAWN_M2*2+playerIsBlack);
     if(!IS_EMPTY(pawn_to_timeout_en_pass.type)){
          position[pawn_to_timeout_en_pass.y][pawn_to_timeout_en_pass.x] = PAWN*2+playerIsBlack;
     }
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
     /*if(head == root){
          printf("getPossibleMoves returned null");
     }*/

     return root;
}

#pragma endregion

#pragma region // Computer Move Choosing

int eval_board(board to_eval){
     int eval = 0;
     for (int y = 0; y < BOARD_SIZE; y++)
     {
          for (int x = 0; x < BOARD_SIZE; x++)
          {
               int piece = to_eval[y][x];
               //Check if square is empty
               if (IS_EMPTY(piece))
               {
                    continue;
               }
               // Could optimize if necessary, but this is more readable
               if(IS_BLACK(piece) == COMPUTER_IS_WHITE){
                    eval -= pointValues[GET_TYPE(piece)];
               }else{
                    eval += pointValues[GET_TYPE(piece)];

               }
          }
     }
     return eval;
}
int minimax_odd(int depth, board current);
int minimax_even(int depth, board current){
     if(depth == 0){
          //printf("Evaled board at %d\n",eval_board(current));
          return eval_board(current);
     }
     boardLinkedList *possibleMoves = getPossibleMovesFromBoard(current,COMPUTER_IS_WHITE);
     boardLinkedList *head = possibleMoves;
     int min =  100000;
     while(head->next != 0){
          int eval = minimax_odd(depth-1,head->current);
          if(eval < min)
               min = eval;
          head = head->next;
     }
     freeBLL(possibleMoves);
     return min;
}

int minimax_odd(int depth, board current){
     if(depth == 0){
          //printf("Evaled board at %d\n",eval_board(current));
          return eval_board(current);
     }
     boardLinkedList *possibleMoves = getPossibleMovesFromBoard(current,!COMPUTER_IS_WHITE);
     boardLinkedList *head = possibleMoves;
     //printf("start minimax with depth %d and h->n = %d\n",depth,COMPUTER_IS_WHITE);
     int max = -100000;
     
     while(head->next != 0){
          int eval = minimax_even(depth-1,head->current);
          if(eval > max)
               max = eval;
          head = head->next;
     }
     freeBLL(possibleMoves);
     return max;
}



int computer_move(board current){
     boardPosition state = {.blackMove = !COMPUTER_IS_WHITE, .eval = 0x0};
     memcpy(state.position,current,sizeof(board));
     boardLinkedList *possibleMoves = getPossibleMovesFromBoard(state.position,state.blackMove);
     if(possibleMoves->next == 0){
          //printf("0 comp moves generated");
          free(possibleMoves);
          return 1;
     }
     //int index = rand() % BLL_length(possibleMoves);
     //BLL_item(possibleMoves,index,current);
     boardLinkedList *head = possibleMoves;
     int max = -100000;
     while(head->next != 0){
          board to_explore;
          memcpy(to_explore,head->current,sizeof(board));
          int eval = minimax_even((DEPTH-1)*2,to_explore);
          if(eval > max){
               max = eval;
               //printf("Y - eval: %d\n",eval);
               memcpy(current,head->current,sizeof(board));
          }else if(eval == max && rand() % 100 > 20){
               memcpy(current,head->current,sizeof(board));
          }
          head = head->next;
     }
     printf("evaled at %d\n",max);
     freeBLL(possibleMoves);
     return 0;
}

#pragma endregion


#pragma region // Human Input Processing

piece read_piece(){
     char p, x, y;
     piece ret;
     printf("Make your move: ");
     scanf("\n%c%c%c",&p,&x,&y);
     //printf("Received p: %c, x: %c, y: %c\n",p,x,y);
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
     //printf("Moved to: %d, x: %d, y: %d\n",ret.type,ret.x,ret.y);
     
     return ret;
}


int human_move(board current){
     boardLinkedList *possibleMoves = getPossibleMovesFromBoard(current,COMPUTER_IS_WHITE);
     if(possibleMoves->next == 0){
          freeBLL(possibleMoves);
          return 1;
     }
     while(true){ // repeat so players can retry if they make an illegal move
          boardLinkedList *head = possibleMoves;
          piece to_move = read_piece();
          if(to_move.x < 0 || to_move.x >= BOARD_SIZE || to_move.y < 0 || to_move.y >= 8 || to_move.type < 0 || to_move.type >= NUM_PIECE_TYPES || (GET_TYPE(current[to_move.y][to_move.x]) == to_move.type && IS_BLACK(current[to_move.y][to_move.x]) == COMPUTER_IS_WHITE)){
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
                    return 0;
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
     return 0;
}

#pragma endregion


int main()
{
     setlocale(LC_ALL, "");     
     srand(time(NULL));
     //printBLL(initialMoves);
     board current = INITIAL_BOARD_STATE;
     HISTORY = (boardLinkedList *) malloc(sizeof(boardLinkedList));
     HISTORY_HEAD = HISTORY;
     HISTORY_HEAD = appendToBLL(current,HISTORY_HEAD);
     if(!COMPUTER_IS_WHITE){
          printBoard(current);
          human_move(current);
          printBoard(current);
          HISTORY_HEAD = appendToBLL(current,HISTORY_HEAD);
          //sleep(1);

     }
     while(true){
          if(computer_move(current)){
               piece king = findPiece(current, KING*2+1-COMPUTER_IS_WHITE);
               if(IS_EMPTY(king.type)){
                    king = findPiece(current, U_KING*2+1-COMPUTER_IS_WHITE);
               }
               if(inCheck(current,king))
                    printf("Checkmate - YOU WIN!!!\n");
               else
                    printf("Stalemate - Draw\n");
               break;
          }
          HISTORY_HEAD = appendToBLL(current,HISTORY_HEAD);
          printBoard(current);
          if(human_move(current)){
               piece king = findPiece(current, KING*2+COMPUTER_IS_WHITE);
               if(IS_EMPTY(king.type)){
                    king = findPiece(current, U_KING*2+COMPUTER_IS_WHITE);
               }
               if(inCheck(current,king))
                    printf("Checkmate - you lose :(\n");
               else
                    printf("Stalemate - Draw\n");
               break;
          }
          HISTORY_HEAD = appendToBLL(current,HISTORY_HEAD);
          printBoard(current);
          //sleep(1);
     }


     printf("Final Position: \n");
     printBoard(current);
     freeBLL(HISTORY);
     //printf("\n%d\n",inCheck(current,findPiece(current,U_KING*2+1)));
     return 0;
}