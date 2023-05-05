#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>      /* wint_t */
#include <locale.h>
#include "possible move generator.c"
#include "initial board.c"

int main()
{
     setlocale(LC_ALL, "");     
     srand(time(NULL));
     //printBLL(initialMoves);
     board current = INITIAL_BOARD_STATE;
     boardLinkedList *history = (boardLinkedList *) malloc(sizeof(boardLinkedList));

     appendToBLL(current,history);
     if(!COMPUTER_IS_WHITE){
          printBoard(current);
          human_move(current);
          printBoard(current);
          sleep(1);

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
          appendToBLL(current,history);
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
          appendToBLL(current,history);
          printBoard(current);
          sleep(1);
     }


     printf("Final Position: \n");
     printBoard(current);
     freeBLL(history);
     //printf("\n%d\n",inCheck(current,findPiece(current,U_KING*2+1)));
     return 0;
}