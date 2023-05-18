


#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include "headers/globals.h"
#include "headers/structs.h"
#include "headers/bll.h"
#include "headers/defs.h"
#include "headers/board_piece.h"
#include "headers/moves.h"
#include "headers/input.h"
#include "headers/bot.h"
#include "headers/macros.h"
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
          printBoard(current);
          human_move(current);
          system("clear");
          printBoard(current);
          HISTORY_HEAD = appendToBLL(current,HISTORY_HEAD);
          //sleep(1);

     }
     while(true){
          if(computer_move(current,!COMPUTER_IS_WHITE)){
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
          system("clear");
          printBoard(current);
          sleep(0.9);
          if(computer_move(current,COMPUTER_IS_WHITE)){
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
          system("clear");
          printBoard(current);
          sleep(0.9);
     }


     printf("Final Position: \n");
     printBoard(current);
     freeBLL(HISTORY);
     //printf("\n%d\n",inCheck(current,findPiece(current,U_KING*2+1)));
     return 0;
}