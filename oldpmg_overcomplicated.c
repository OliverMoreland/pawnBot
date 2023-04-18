#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>      /* wint_t */
#include <locale.h>
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

const int pieceChars[19] = {0x0020,0x2659,0x265F, 0x2658,0x265E, 0x2657,0x265D, 0x2656,0x265C, 0x2655,0x265B, 0x2654,0x265A, 0x2659,0x265F, 0x2654,0x265A, 0x2656,0x265C};

const short pointValues[6] = {1, 3, 3, 5, 9};

typedef int board[BOARD_SIZE][BOARD_SIZE];

struct boardPositionLinkedList;
typedef struct boardPosition
{
     bool blackMove;
     short eval;
     board position;
     struct boardPositionLinkedList *children;
} boardPosition;

typedef struct boardPositionLinkedListSegment
{
     boardPosition current;
     struct boardPositionLinkedList *next;
} boardPositionLinkedList;


typedef struct boardLinkedListSegment
{
     board current;
     struct boardLinkedListSegment *next;
} boardLinkedListSegment;


typedef struct boardLinkedList
{
     boardLinkedListSegment *root;
     int length;
} boardLinkedList;


typedef struct piece {
     int x;
     int y;
     int type;
} piece;

void freeBLLS(boardLinkedListSegment *toFree){
     if(toFree->next != 0)
     {
          freeBLL(toFree->next);
     }
     free(toFree);
     return;
}


boardLinkedListSegment* appendToBLLS(board toAppend,boardLinkedListSegment *head)
{
     
     memcpy( head->current,toAppend, sizeof( head->current ) );
     head->next = (boardLinkedListSegment *)malloc(sizeof(boardLinkedListSegment));
     head = head->next;
     head->next = 0;

     return head;
}


void printBoard (board toPrint){
     for (int y = 0; y < BOARD_SIZE; y++)
     {
          for (int x = 0; x < BOARD_SIZE; x++)
          {
               if(y&1 ^ x&1){
                    printf("\x1b[47m %lc ", (wint_t)pieceChars[toPrint[y][x]+1]);
               }else{
                    printf("\x1b[40m %lc ", (wint_t)pieceChars[toPrint[y][x]+1]);
               }
          }
          printf("\x1b[40m \n");
     }
     
}

void printBLLS(boardLinkedListSegment root){
    setlocale(LC_ALL, "");     
     if(root.next == 0){
          printf("\nDONE\n");
          return;
     }
     printf("\n\n\nBoard\n\n");
     printBoard(root.current);
     printBLL(*(root.next));


}


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
                         GET_TYPE(pieceToCheck) != BISHOP 
                         ||
                         GET_TYPE(pieceToCheck) != QUEEN
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
                         ||
                         GET_TYPE(pieceToCheck) != QUEEN
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

boardLinkedListSegment* movePiece(board newBoard,boardLinkedListSegment *head, piece king){
     if(inCheck(newBoard,king))
          return head;
     return appendToBLL(newBoard, head);
}
/*
     This is a group of functions that define how the pieces move (promotion and castling not yet implemented)
*/





boardLinkedListSegment *getPossibleMovesPawn(board position, int x, int y, boardLinkedListSegment *head, bool playerIsBlack, piece king)
{
     int direction = FORWARD(position[y][x]);
     board newPosition;
     memcpy(newPosition,position,sizeof(newPosition));

     if(IS_EMPTY(position[y+direction][x]))
     {
          //Standard Movement
          newPosition[y+direction][x] = 1-playerIsBlack;
          newPosition[y][x] = -1;
          head = movePiece(newPosition,head,king);
          memcpy(newPosition,position,sizeof(newPosition));
          //Double Movement
          if(y == 6-5*playerIsBlack &&  IS_EMPTY(position[y+direction*2][x]))
          {
               newPosition[y+direction*2][x] = 13-playerIsBlack;
               newPosition[y][x] = -1;
               head = movePiece(newPosition,head,king);
               memcpy(newPosition,position,sizeof(newPosition));

          }
     }
     //Capture
     if(!IS_EMPTY(position[y+direction][x-1]) && (IS_BLACK(position[y+direction][x-1]) ^ playerIsBlack))
     {
          newPosition[y+direction][x-1] = 1-playerIsBlack;
          newPosition[y][x] = -1;
          head = movePiece(newPosition,head,king);
          memcpy(newPosition,position,sizeof(newPosition));     
     }
     if(!IS_EMPTY(position[y+direction][x+1]) && (IS_BLACK(position[y+direction][x+1]) ^ playerIsBlack))
     {
          newPosition[y+direction][x+1] = 1-playerIsBlack;
          newPosition[y][x] = -1;
          head = movePiece(newPosition,head,king);
          memcpy(newPosition,position,sizeof(newPosition));

     }
     //En Passante
     if(GET_TYPE(position[y][x-1]) == PAWN_M2 && IS_EMPTY(position[y+direction][x-1]) && (IS_BLACK(position[y][x-1]) ^ playerIsBlack))
     {
          newPosition[y+direction][x-1] = position[y][x];
          newPosition[y][x] = -1;
          newPosition[y][x-1] = -1;
          head = movePiece(newPosition,head,king);
          memcpy(newPosition,position,sizeof(newPosition));

     }
     if(GET_TYPE(position[y][x+1]) == PAWN_M2 && IS_EMPTY(position[y+direction][x+1]) && (IS_BLACK(position[y][x+1]) ^ playerIsBlack))
     {
          newPosition[y+direction][x+1] = position[y][x];
          newPosition[y][x] = -1;
          newPosition[y][x+1] = -1;
          head = movePiece(newPosition,head,king);
          memcpy(newPosition,position,sizeof(newPosition));     
     }


     return head;

}    
boardLinkedListSegment *getPossibleMovesKnight(board position, int x, int y, boardLinkedListSegment *head, bool playerIsBlack, piece king)
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
boardLinkedListSegment *getPossibleMovesBishop(board position, int x, int y, boardLinkedListSegment *head, bool playerIsBlack, piece king)
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
boardLinkedListSegment *getPossibleMovesRook(board position, int x, int y, boardLinkedListSegment *head, bool playerIsBlack, piece king)
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
               printf("%i, %i\n",xToCheck,yToCheck);
               if(!IS_EMPTY(position[yToCheck][xToCheck]))
               {
                    break;
               }
          }

     }
     
     return head; 
}      
boardLinkedListSegment *getPossibleMovesQueen(board position, int x, int y, boardLinkedListSegment *head, bool playerIsBlack, piece king)
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
               newPosition[yToCheck][xToCheck] = position[y][x];
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
               newPosition[yToCheck][xToCheck] = 7-playerIsBlack;
               newPosition[y][x] = -1;
               head = movePiece(newPosition,head,king);
               printf("%i, %i\n",xToCheck,yToCheck);
               if(!IS_EMPTY(position[yToCheck][xToCheck]))
               {
                    break;
               }
          }

     }
 
     
     return head;
}      
boardLinkedListSegment *getPossibleMovesKing(board position, int x, int y, boardLinkedListSegment *head, bool playerIsBlack, piece king)
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
boardLinkedListSegment *getPossibleMovesUKing(board position, int x, int y, boardLinkedListSegment *head, bool playerIsBlack, piece king)
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




boardLinkedListSegment *(*functionsThatGetPossibleMoves[])(board position, int x, int y, boardLinkedListSegment *head, bool playerIsBlack, piece king) = {
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
     boardLinkedListSegment *root;
     boardLinkedListSegment *head;
     root = (boardLinkedListSegment *)malloc(sizeof(boardLinkedListSegment));
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
     

     return root;
}



int main()
{
     boardPosition initial = {.blackMove = false, .position = INITIAL_BOARD_STATE, .eval = 0x7000};
     boardLinkedListSegment *initialMoves = getPossibleMovesFromBoard(initial.position,initial.blackMove);
     printBLL(*initialMoves);
     // boardLinkedListSegment *secondMoves = getPossibleMovesFromBoard(initialMoves->next->current,false);
     
     // printf("\n\nSecond Move from initialMoves next\n\n");
     // boardLinkedListSegment *thirdMoves = getPossibleMovesFromBoard(secondMoves->next->current,true);
     // printBLL(*secondMoves);
     // freeBLL(secondMoves);
     // printf("\n\nThird Move from secondMoves next\n\n");
     // printBLL(*thirdMoves);
     // freeBLL(thirdMoves);

     

     freeBLL(initialMoves);
     // printf("%i",sizeof(boardPosition));
}
