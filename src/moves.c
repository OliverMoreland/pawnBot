
#include "structs.h"
#include "globals.h"
#include "macros.h"
#include "board_piece.h"
#include "defs.h"
#include "bll.h"
#include <stdio.h>
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
          if(y+direction == BOARD_SIZE-1 || y+direction == 0){
               
               newPosition[y+direction][x] = KNIGHT*2+playerIsBlack;
               head = move_piece(newPosition,head,king);
               newPosition[y+direction][x] = BISHOP*2+playerIsBlack;
               head = move_piece(newPosition,head,king);
               newPosition[y+direction][x] = ROOK*2+playerIsBlack;
               head = move_piece(newPosition,head,king);
               newPosition[y+direction][x] = QUEEN*2+playerIsBlack;
               head = move_piece(newPosition,head,king);
               memcpy(newPosition,position,sizeof(board));
          }else {
               newPosition[y+direction][x] = PAWN*2+playerIsBlack;
               head = move_piece(newPosition,head,king);
               memcpy(newPosition,position,sizeof(board));
          }
          //Double Movement
          if(y == 6-5*playerIsBlack &&  IS_EMPTY(position[y+direction*2][x]))
          {
               newPosition[y+direction*2][x] = PAWN_M2*2+playerIsBlack;
               newPosition[y][x] = -1;
               head = move_piece(newPosition,head,king);
               memcpy(newPosition,position,sizeof(board));

          }
     }
     //Capture
     if(!IS_EMPTY(position[y+direction][x-1]) && (IS_BLACK(position[y+direction][x-1]) ^ playerIsBlack) && x-1 != 0)
     {
          newPosition[y][x] = -1;
          //Standard Movement
          if(y+direction == BOARD_SIZE-1 || y+direction == 0){
               
               newPosition[y+direction][x-1] = KNIGHT*2+playerIsBlack;
               head = move_piece(newPosition,head,king);
               newPosition[y+direction][x-1] = BISHOP*2+playerIsBlack;
               head = move_piece(newPosition,head,king);
               newPosition[y+direction][x-1] = ROOK*2+playerIsBlack;
               head = move_piece(newPosition,head,king);
               newPosition[y+direction][x-1] = QUEEN*2+playerIsBlack;
               head = move_piece(newPosition,head,king);
               memcpy(newPosition,position,sizeof(board));
          }else {
               newPosition[y+direction][x-1] = PAWN*2+playerIsBlack;
               head = move_piece(newPosition,head,king);
               memcpy(newPosition,position,sizeof(board));
          }
     }
     if(!IS_EMPTY(position[y+direction][x+1]) && (IS_BLACK(position[y+direction][x+1]) ^ playerIsBlack) && x+1 != BOARD_SIZE)
     {
          newPosition[y][x] = -1;
          //Standard Movement
          if(y+direction == BOARD_SIZE-1 || y+direction == 0){
               
               newPosition[y+direction][x+1] = KNIGHT*2+playerIsBlack;
               head = move_piece(newPosition,head,king);
               newPosition[y+direction][x+1] = BISHOP*2+playerIsBlack;
               head = move_piece(newPosition,head,king);
               newPosition[y+direction][x+1] = ROOK*2+playerIsBlack;
               head = move_piece(newPosition,head,king);
               newPosition[y+direction][x+1] = QUEEN*2+playerIsBlack;
               head = move_piece(newPosition,head,king);
               memcpy(newPosition,position,sizeof(board));
          }else {
               newPosition[y+direction][x+1] = PAWN*2+playerIsBlack;
               head = move_piece(newPosition,head,king);
               memcpy(newPosition,position,sizeof(board));
          }

     }
     //En Passante
     if(GET_TYPE(position[y][x-1]) == PAWN_M2 && IS_EMPTY(position[y+direction][x-1]) && (IS_BLACK(position[y][x-1]) ^ playerIsBlack) && x-1 != 0)
     {
          newPosition[y+direction][x-1] = PAWN*2+playerIsBlack;
          newPosition[y][x] = -1;
          newPosition[y][x-1] = -1;
          head = move_piece(newPosition,head,king);
          memcpy(newPosition,position,sizeof(board));

     }
     if(GET_TYPE(position[y][x+1]) == PAWN_M2 && IS_EMPTY(position[y+direction][x+1]) && (IS_BLACK(position[y][x+1]) ^ playerIsBlack) && x+1 != BOARD_SIZE)
     {
          newPosition[y+direction][x+1] = PAWN*2+playerIsBlack;
          newPosition[y][x] = -1;
          newPosition[y][x+1] = -1;
          head = move_piece(newPosition,head,king);
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
          if(!IS_EMPTY(position[yToCheck][xToCheck]) && !(IS_BLACK(position[yToCheck][xToCheck]) ^ playerIsBlack))
          {
               continue;
          }
          memcpy(newPosition,position,sizeof(board));
          newPosition[yToCheck][xToCheck] = position[y][x];
          newPosition[y][x] = -1;
          
          head = move_piece(newPosition,head,king);

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
               memcpy(newPosition,position,sizeof(board));
               newPosition[yToCheck][xToCheck] = position[y][x];
               newPosition[y][x] = -1;
               head = move_piece(newPosition,head,king);
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
               memcpy(newPosition,position,sizeof(board));
               newPosition[yToCheck][xToCheck] = ROOK*2+playerIsBlack;
               newPosition[y][x] = -1;
               head = move_piece(newPosition,head,king);
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
               memcpy(newPosition,position,sizeof(board));
               newPosition[yToCheck][xToCheck] = QUEEN*2+playerIsBlack;
               newPosition[y][x] = -1;
               head = move_piece(newPosition,head,king);
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
               memcpy(newPosition,position,sizeof(board));
               newPosition[yToCheck][xToCheck] = QUEEN*2+playerIsBlack;
               newPosition[y][x] = -1;
               head = move_piece(newPosition,head,king);
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
          memcpy(newPosition,position,sizeof(board));
          newPosition[yToCheck][xToCheck] = KING*2+playerIsBlack;
          newPosition[y][x] = -1;
          head = move_piece(newPosition,head,king);

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
          memcpy(newPosition,position,sizeof(board));
          newPosition[yToCheck][xToCheck] = KING*2+playerIsBlack;
          newPosition[y][x] = -1;
          head = move_piece(newPosition,head,king);

     }
     
     
     return head;

}
boardLinkedList *getPossibleMovesUKing(board position, int x, int y, boardLinkedList *head, bool playerIsBlack, piece king)
{
     board newPosition;
     // Do Normal King Moves
     head = getPossibleMovesKing(position, x, y, head, playerIsBlack,king);

     if(!in_check(position,king)){ // No castling out of check
          // Queenside Castle
          if(GET_TYPE(position[y][0]) == U_ROOK && IS_EMPTY(position[y][x-1]) && IS_EMPTY(position[y][x-2])){
               memcpy(newPosition,position,sizeof(board));
               newPosition[y][x-1] = KING*2+IS_BLACK(king.type);
               king.x--;
               newPosition[y][x] = -1;
               if(!in_check(newPosition,king))
               {
                    newPosition[y][x-2] = newPosition[y][x-1];
                    king.x--;
                    newPosition[y][x-1] = -1;
                    if(!in_check(newPosition,king)){
                         newPosition[y][x-1] = newPosition[y][0];
                         newPosition[y][0] = -1;
                         head = move_piece(newPosition,head,king);
                    }
               }
               
          }
          // Kingside Castle
          if(GET_TYPE(position[y][BOARD_SIZE-1]) == U_ROOK && IS_EMPTY(position[y][x+1]) && IS_EMPTY(position[y][x+2])){
               board newPosition;
               memcpy(newPosition,position,sizeof(board));
               newPosition[y][x+1] = KING*2+IS_BLACK(king.type);
               king.x++;
               newPosition[y][x] = -1;
               if(!in_check(newPosition,king))
               {
                    newPosition[y][x+2] = newPosition[y][x+1];
                    king.x++;
                    newPosition[y][x+1] = -1;
                    if(!in_check(newPosition,king)){
                         newPosition[y][x+1] = newPosition[y][0];
                         newPosition[y][BOARD_SIZE-1] = -1;
                         head = move_piece(newPosition,head,king);
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

coordLL *get_possible_moves_from_piece(board position, int x, int y, bool playerIsBlack){

     boardLinkedList *root;
     boardLinkedList *head;
     coordLL *moves = (coordLL *)malloc(sizeof(coordLL));
     coordLL *moves_head = moves;
     moves->next = 0;
     root = (boardLinkedList *)malloc(sizeof(boardLinkedList));
     root->next = 0;
     head = root;
     piece pawn_to_timeout_en_pass = find_piece(position, PAWN_M2*2+playerIsBlack);
     if(!IS_EMPTY(pawn_to_timeout_en_pass.type)){
          position[pawn_to_timeout_en_pass.y][pawn_to_timeout_en_pass.x] = PAWN*2+playerIsBlack;
     }
     piece king = find_piece(position, KING*2+playerIsBlack);
     if(IS_EMPTY(king.type)){
          king = find_piece(position, U_KING*2+playerIsBlack);
     }
     if(IS_EMPTY(king.type)){
          return moves;
     }
     functionsThatGetPossibleMoves[GET_TYPE(position[y][x])](position, x, y, head, playerIsBlack,king);
     while(head->next != 0){
          int cx,cy;
          for (cy = 0; cy < BOARD_SIZE; cy++){
               bool br = false;
               for (cx = 0; cx < BOARD_SIZE; cx++){
                    int piece_to_check = head->current[cy][cx];
                    int to_move = position[y][x];
                    if(
                         IS_BLACK(piece_to_check) == IS_BLACK(to_move)
                         &&
                         (
                              GET_TYPE(piece_to_check) == GET_TYPE(to_move) 
                              || 
                              (GET_TYPE(piece_to_check) == PAWN && GET_TYPE(to_move) == PAWN_M2) 
                              || 
                              (GET_TYPE(piece_to_check) == PAWN_M2 && GET_TYPE(to_move) == PAWN) 
                              || 
                              (GET_TYPE(piece_to_check) == ROOK && GET_TYPE(to_move) == U_ROOK) 
                              || 
                              (GET_TYPE(piece_to_check) == KING && GET_TYPE(to_move) == U_KING)
                         )
                         &&
                         position[cy][cx] != to_move
                    )
                    {
                         moves_head = appendToCLL(cx,cy,moves_head);
                         br = true;

                         break;
                    }
               }
               if(br){
                    break;
               }
          }
          head = head->next;

     }
     return moves;
}

boardLinkedList *get_possible_moves_from_board(board position, bool playerIsBlack)
{

     boardLinkedList *root;
     boardLinkedList *head;
     root = (boardLinkedList *)malloc(sizeof(boardLinkedList));
     root->next = 0;
     head = root;
     piece pawn_to_timeout_en_pass = find_piece(position, PAWN_M2*2+playerIsBlack);
     if(!IS_EMPTY(pawn_to_timeout_en_pass.type)){
          position[pawn_to_timeout_en_pass.y][pawn_to_timeout_en_pass.x] = PAWN*2+playerIsBlack;
     }
     piece king = find_piece(position, KING*2+playerIsBlack);
     if(IS_EMPTY(king.type)){
          king = find_piece(position, U_KING*2+playerIsBlack);
     }
     if(IS_EMPTY(king.type)){
          return root;
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
               if (IS_BLACK(cpiece) ^ playerIsBlack)
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

