
#include "headers/structs.h"
#include "headers/globals.h"
#include "headers/macros.h"
#include "headers/board_piece.h"
#include "headers/defs.h"

#include <string.h>
#include <stdlib.h>

boardLinkedList *getPossibleMovesPawn(board position, int x, int y, boardLinkedList *head, bool playerIsBlack, piece king)
{
     int direction = FORWARD(position[y][x]);
     board newPosition;
     memcpy(newPosition,position,sizeof(board));

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
               memcpy(newPosition,position,sizeof(board));
          }else {
               newPosition[y+direction][x] = PAWN*2+playerIsBlack;
               head = movePiece(newPosition,head,king);
               memcpy(newPosition,position,sizeof(board));
          }
          //Double Movement
          if(y == 6-5*playerIsBlack &&  IS_EMPTY(position[y+direction*2][x]))
          {
               newPosition[y+direction*2][x] = PAWN_M2*2+playerIsBlack;
               newPosition[y][x] = -1;
               head = movePiece(newPosition,head,king);
               memcpy(newPosition,position,sizeof(board));

          }
     }
     //Capture
     if(!IS_EMPTY(position[y+direction][x-1]) && (GET_COLOR(position[y+direction][x-1]) ^ playerIsBlack) && x-1 != 0)
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
               memcpy(newPosition,position,sizeof(board));
          }else {
               newPosition[y+direction][x-1] = PAWN*2+playerIsBlack;
               head = movePiece(newPosition,head,king);
               memcpy(newPosition,position,sizeof(board));
          }
     }
     if(!IS_EMPTY(position[y+direction][x+1]) && (GET_COLOR(position[y+direction][x+1]) ^ playerIsBlack) && x+1 != BOARD_SIZE)
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
               memcpy(newPosition,position,sizeof(board));
          }else {
               newPosition[y+direction][x+1] = PAWN*2+playerIsBlack;
               head = movePiece(newPosition,head,king);
               memcpy(newPosition,position,sizeof(board));
          }

     }
     //En Passante
     if(GET_TYPE(position[y][x-1]) == PAWN_M2 && IS_EMPTY(position[y+direction][x-1]) && (GET_COLOR(position[y][x-1]) ^ playerIsBlack) && x-1 != 0)
     {
          newPosition[y+direction][x-1] = PAWN*2+playerIsBlack;
          newPosition[y][x] = -1;
          newPosition[y][x-1] = -1;
          head = movePiece(newPosition,head,king);
          memcpy(newPosition,position,sizeof(board));

     }
     if(GET_TYPE(position[y][x+1]) == PAWN_M2 && IS_EMPTY(position[y+direction][x+1]) && (GET_COLOR(position[y][x+1]) ^ playerIsBlack) && x+1 != BOARD_SIZE)
     {
          newPosition[y+direction][x+1] = PAWN*2+playerIsBlack;
          newPosition[y][x] = -1;
          newPosition[y][x+1] = -1;
          head = movePiece(newPosition,head,king);
          memcpy(newPosition,position,sizeof(board));     
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
          if(!IS_EMPTY(position[yToCheck][xToCheck]) && !(GET_COLOR(position[yToCheck][xToCheck]) ^ playerIsBlack))
          {
               continue;
          }
          memcpy(newPosition,position,sizeof(board));
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
               if(!IS_EMPTY(position[yToCheck][xToCheck]) && !(GET_COLOR(position[yToCheck][xToCheck]) ^ playerIsBlack))
               {
                    break;
               }
               memcpy(newPosition,position,sizeof(board));
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
               if(!IS_EMPTY(position[yToCheck][xToCheck]) && !(GET_COLOR(position[yToCheck][xToCheck]) ^ playerIsBlack))
               {
                    break;
               }
               memcpy(newPosition,position,sizeof(board));
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
               if(!IS_EMPTY(position[yToCheck][xToCheck]) && !(GET_COLOR(position[yToCheck][xToCheck]) ^ playerIsBlack))
               {
                    break;
               }
               memcpy(newPosition,position,sizeof(board));
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
               if(!IS_EMPTY(position[yToCheck][xToCheck]) && !(GET_COLOR(position[yToCheck][xToCheck]) ^ playerIsBlack))
               {
                    break;
               }
               memcpy(newPosition,position,sizeof(board));
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
          if(!IS_EMPTY(position[yToCheck][xToCheck]) && !(GET_COLOR(position[yToCheck][xToCheck]) ^ playerIsBlack))
          {
               continue;
          }
          king.x = xToCheck;
          king.y = yToCheck;
          memcpy(newPosition,position,sizeof(board));
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

          if(!IS_EMPTY(position[yToCheck][xToCheck]) && !(GET_COLOR(position[yToCheck][xToCheck]) ^ playerIsBlack))
          {
               continue;
          }
          king.x = xToCheck;
          king.y = yToCheck;
          memcpy(newPosition,position,sizeof(board));
          newPosition[yToCheck][xToCheck] = KING*2+playerIsBlack;
          newPosition[y][x] = -1;
          head = movePiece(newPosition,head,king);

     }
     
     
     return head;

}
boardLinkedList *getPossibleMovesUKing(board position, int x, int y, boardLinkedList *head, bool playerIsBlack, piece king)
{
     board newPosition;
     // Do Normal King Moves
     head = getPossibleMovesKing(position, x, y, head, playerIsBlack,king);

     if(!inCheck(position,king)){ // No castling out of check
          // Queenside Castle
          if(GET_TYPE(position[y][0]) == U_ROOK && IS_EMPTY(position[y][x-1]) && IS_EMPTY(position[y][x-2])){
               memcpy(newPosition,position,sizeof(board));
               newPosition[y][x-1] = KING*2+GET_COLOR(king.type);
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
               memcpy(newPosition,position,sizeof(board));
               newPosition[y][x+1] = KING*2+GET_COLOR(king.type);
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

boardLinkedList *getPossibleMovesFromPiece(board position, int x, int y, boardLinkedList *head, bool playerIsBlack, piece king){
     functionsThatGetPossibleMoves[GET_TYPE(position[y][x])](position, x, y, head, playerIsBlack,king);
}

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
               int cpiece = position[y][x];
               //Check if square is empty
               if (IS_EMPTY(cpiece))
               {
                    continue;
               }
               //Check if piece belongs to opposite player
               if (GET_COLOR(cpiece) ^ playerIsBlack)
               {
                    continue;

               }

               head = functionsThatGetPossibleMoves[GET_TYPE(cpiece)](position, x, y, head, playerIsBlack,king);
               
          }
     }
     /*if(head == root){
          printf("getPossibleMoves returned null");
     }*/

     return root;
}
