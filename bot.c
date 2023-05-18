#include "headers/structs.h"
#include "headers/defs.h"
#include "headers/globals.h"
#include "headers/macros.h"
#include "headers/moves.h"
#include "headers/bll.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
const int POINT_VALUES[9] = {1, 3, 3, 5, 9,1000,1,1000,5};

int eval_board(board to_eval, bool computer_is_black){
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
               if(IS_BLACK(piece) == computer_is_black){
                    eval += POINT_VALUES[GET_TYPE(piece)];
               }else{
                    eval -= POINT_VALUES[GET_TYPE(piece)];

               }
          }
     }
     return eval;
}
int minimax_odd(int depth, board current, bool computer_is_black);
int minimax_even(int depth, board current, bool computer_is_black){
     if(depth == 0){
          //printf("Evaled board at %d\n",eval_board(current));
          return eval_board(current,computer_is_black);
     }
     boardLinkedList *possibleMoves = getPossibleMovesFromBoard(current,!computer_is_black);
     boardLinkedList *head = possibleMoves;
     int min =  100000;
     while(head->next != 0){
          int eval = minimax_odd(depth-1,head->current,computer_is_black);
          if(eval < min)
               min = eval;
          head = head->next;
     }
     freeBLL(possibleMoves);
     return min;
}

int minimax_odd(int depth, board current, bool computer_is_black){
     if(depth == 0){
          //printf("Evaled board at %d\n",eval_board(current));
          return eval_board(current,computer_is_black);
     }
     boardLinkedList *possibleMoves = getPossibleMovesFromBoard(current,computer_is_black);
     boardLinkedList *head = possibleMoves;
     //printf("start minimax with depth %d and h->n = %d\n",depth,COMPUTER_IS_WHITE);
     int max = -100000;
     
     while(head->next != 0){
          int eval = minimax_even(depth-1,head->current,computer_is_black);
          if(eval > max)
               max = eval;
          head = head->next;
     }
     freeBLL(possibleMoves);
     return max;
}



int computer_move(board current, bool computer_is_black){
     boardPosition state = {.blackMove = computer_is_black, .eval = 0x0};
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
          int eval = minimax_even((DEPTH-1)*2,to_explore,computer_is_black);
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