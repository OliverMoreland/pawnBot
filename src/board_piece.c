#include "structs.h"
#include <wchar.h>
#include "globals.h"
#include "defs.h"
#include <stdlib.h>
#include <stdio.h>
#include "bll.h"
#include "moves.h"
#include "macros.h"
//Flipped for different terminals: 
const int PIECE_CHARS[19] = {0x0020,0x265F,0x2659, 0x265E,0x2658, 0x265D,0x2657, 0x265C,0x2656, 0x265B,0x2655, 0x265A,0x2654, 0x265F,0x2659, 0x265A,0x2654, 0x265C,0x2656};
//const int PIECE_CHARS[19] = {0x0020,0x2659,0x265F, 0x2658,0x265E, 0x2657, 0x265D, 0x2656,0x265C, 0x2655,0x265B, 0x2654,0x265A, 0x2659,0x265F, 0x2654,0x265A, 0x2656,0x265C};

void print_board (board toPrint){
     printf("   a  b  c  d  e  f  g  h \n");
     for (int y = 0; y < BOARD_SIZE; y++)
     {
          printf("%d ",8-y);
          for (int x = 0; x < BOARD_SIZE; x++)
          {
               //printf("%d ",toPrint[y][x]+1);

               if(y&1 ^ x&1){
                    printf("\x1b[40m %lc ", (wint_t)PIECE_CHARS[toPrint[y][x]+1]);
               }else{
                    printf("\x1b[41m %lc ", (wint_t)PIECE_CHARS[toPrint[y][x]+1]);
               }
          }
          printf("\x1b[40m %d\n",8-y);
     }
     printf("   a  b  c  d  e  f  g  h \n");

}

void print_board_copy (board toPrint){
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
          printf(" }, \\ \n");
     }
     printf("\n\n");

}



piece find_piece(board position, int pieceToFind){
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

bool in_check(board position,piece king){
     if(GET_TYPE(king.type) != KING && GET_TYPE(king.type) != U_KING){
          return false;
     }
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
          !(GET_COLOR(position[yToCheck][xToCheck]) ^ GET_COLOR(king.type))
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

                    !(GET_COLOR(pieceToCheck) ^ GET_COLOR(king.type))
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

                    !(GET_COLOR(pieceToCheck) ^ GET_COLOR(king.type))
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
               GET_COLOR(position[king.y+FORWARD(king.type)][king.x-1]) ^ GET_COLOR(king.type)
          )
          ||
          (
               GET_TYPE(position[king.y+FORWARD(king.type)][king.x+1]) == PAWN
               &&
               GET_COLOR(position[king.y+FORWARD(king.type)][king.x+1]) ^ GET_COLOR(king.type)
          )
     ){
          return true;
     }

     return false;
     
}

boardLinkedList* move_piece(board newBoard,boardLinkedList *head, piece king){
     if(in_check(newBoard,king))
     {
          //printf("check ");
          return head;
     }
     return appendToBLL(newBoard, head);
}

bool is_same_board(board b1, board b2){
     for(int i = 0;i < BOARD_SIZE; i++){
          for (int j = 0; j < BOARD_SIZE; j++)
          {
               if(b1[i][j] != b2[i][j]){
                    return false;
               }
          }
          
     }
     return true;
}

bool is_legal_move(board position, board move, bool playerIsBlack){
     boardLinkedList *possibleMoves = get_possible_moves_from_board(position,playerIsBlack);
     boardLinkedList *head = possibleMoves;
     while(head->next != 0){
          if(is_same_board(head->current,move)){
               freeBLL(possibleMoves);
               return true;
          }
          head = head->next;
     }
     freeBLL(possibleMoves);
     return false;
}