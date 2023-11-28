#include "structs.h"
#include "defs.h"
#include "globals.h"
#include "macros.h"
#include "moves.h"
#include "bll.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

const int POINT_VALUES[9] = {1, 3, 3, 5, 9, 1000, 1, 1000, 5};

int eval_board(board to_eval, bool computer_is_black)
{
     int eval = 0;
     for (int y = 0; y < BOARD_SIZE; y++)
     {
          for (int x = 0; x < BOARD_SIZE; x++)
          {
               int piece = to_eval[y][x];
               // Check if square is empty
               if (IS_EMPTY(piece))
               {
                    continue;
               }
               // Could optimize if necessary, but this is more readable
               if (GET_COLOR(piece) == computer_is_black)
               {
                    eval += POINT_VALUES[GET_TYPE(piece)];
               }
               else
               {
                    eval -= POINT_VALUES[GET_TYPE(piece)];
               }
          }
     }
     return eval;
}

int minimax_odd(int depth, board current, bool computer_is_black);
int minimax_even(int depth, board current, bool computer_is_black)
{
     if (depth == 0)
     {
          // printf("Evaled board at %d\n",eval_board(current));
          return eval_board(current, computer_is_black);
     }
     boardLinkedList *possibleMoves = get_possible_moves_from_board(current, !computer_is_black);
     if (possibleMoves->next == 0)
     {
          return eval_board(current, computer_is_black);
     }
     boardLinkedList *head = possibleMoves;
     int min = 100000;
     while (head->next != 0)
     {
          int eval = minimax_odd(depth - 1, head->current, computer_is_black);
          if (eval < min)
               min = eval;
          head = head->next;
     }
     freeBLL(possibleMoves);
     return min;
}
int minimax_odd(int depth, board current, bool computer_is_black)
{
     if (depth == 0)
     {
          // printf("Evaled board at %d\n",eval_board(current));
          return eval_board(current, computer_is_black);
     }
     boardLinkedList *possibleMoves = get_possible_moves_from_board(current, computer_is_black);
     if (possibleMoves->next == 0)
     {
          return eval_board(current, computer_is_black);
     }
     boardLinkedList *head = possibleMoves;
     // printf("start minimax with depth %d and h->n = %d\n",depth,COMPUTER_COLOR);
     int max = -100000;

     while (head->next != 0)
     {
          int eval = minimax_even(depth - 1, head->current, computer_is_black);
          if (eval > max)
               max = eval;
          head = head->next;
     }
     freeBLL(possibleMoves);
     return max;
}

int computer_move(board current)
{
     boardPosition state = {.blackMove = COMPUTER_COLOR, .eval = 0x0};
     memcpy(state.position, current, sizeof(board));
     boardLinkedList *possibleMoves = get_possible_moves_from_board(state.position, state.blackMove);
     if (possibleMoves->next == 0)
     {
          // printf("0 comp moves generated");
          free(possibleMoves);
          return 1;
     }
     // int index = rand() % BLL_length(possibleMoves);
     // BLL_item(possibleMoves,index,current);
     boardLinkedList *head = possibleMoves;
     int max = -100000;
     while (head->next != 0)
     {
          board to_explore;
          memcpy(to_explore, head->current, sizeof(board));
          int eval = minimax_even((DEPTH - 1) * 2, to_explore, COMPUTER_COLOR);
          if (eval > max)
          {
               max = eval;
               // printf("Y - eval: %d\n",eval);
               memcpy(current, head->current, sizeof(board));
          }
          else if (eval == max && rand() % 100 > 20)
          {
               memcpy(current, head->current, sizeof(board));
          }
          head = head->next;
     }
     printf("evaled at %d\n", max);
     freeBLL(possibleMoves);
     return 0;
}

int computer_move2(board current)
{

}

int start_bot_daemon(board initial_board)
{

     
     // Daemon Code From Here: https://lloydrochester.com/post/c/unix-daemon-example/
     int maxfd, fd;
     switch (fork()) // become background process
     {
     case -1:
          return -1;
     case 0:
          break; // child falls through
     default:
          return EXIT_SUCCESS; // parent terminates
     }
     if (setsid() == -1) // become leader of new session
          return -1;
     switch (fork())
     {
     case -1:
          return EXIT_FAILURE;
     case 0:
          break; // child breaks out of case
     default:
          return EXIT_SUCCESS; // parent terminates
     }
     
     close(STDIN_FILENO);

     fd = open("/dev/null", O_RDWR);

     FILE *in = fopen("bot.in","r+");
     FILE *out = fopen("bot.out","r+");
     int log = open("bot.log",O_WRONLY);
     if (fd != STDIN_FILENO)
          return -1;
     if (dup2(log, STDOUT_FILENO) != STDOUT_FILENO)
          return -2;
     if (dup2(log, STDERR_FILENO) != STDERR_FILENO)
          return -3;

     
     printf("BEGIN LOG\n");
     printf("test log\n");

     printf("END LOG\n");

     
     return 0;

}