


#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include "globals.h"
#include "structs.h"
#include "bll.h"
#include "defs.h"
#include "board_piece.h"
#include "moves.h"
#include "input.h"
#include "bot.h"
#include "macros.h"
boardLinkedList *HISTORY;
boardLinkedList *HISTORY_HEAD;

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
          print_board(current);
          human_move(current);
          system("clear");
          print_board(current);
          HISTORY_HEAD = appendToBLL(current,HISTORY_HEAD);
          //sleep(1);

     }
     while(true){
          if(computer_move(current,!COMPUTER_IS_WHITE)){
               piece king = find_piece(current, KING*2+1-COMPUTER_IS_WHITE);
               if(IS_EMPTY(king.type)){
                    king = find_piece(current, U_KING*2+1-COMPUTER_IS_WHITE);
               }
               if(in_check(current,king))
                    printf("Checkmate - YOU WIN!!!\n");
               else
                    printf("Stalemate - Draw\n");
               break;
          }
          HISTORY_HEAD = appendToBLL(current,HISTORY_HEAD);
          system("clear");
          print_board(current);
          sleep(0.9);
          if(computer_move(current,COMPUTER_IS_WHITE)){
               piece king = find_piece(current, KING*2+COMPUTER_IS_WHITE);
               if(IS_EMPTY(king.type)){
                    king = find_piece(current, U_KING*2+COMPUTER_IS_WHITE);
               }
               if(in_check(current,king))
                    printf("Checkmate - you lose :(\n");
               else
                    printf("Stalemate - Draw\n");
               break;
          }
          HISTORY_HEAD = appendToBLL(current,HISTORY_HEAD);
          system("clear");
          print_board(current);
          sleep(0.9);
     }


     printf("Final Position: \n");
     print_board(current);
     freeBLL(HISTORY);
     //printf("\n%d\n",inCheck(current,findPiece(current,U_KING*2+1)));
     return 0;
}