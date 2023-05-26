#include "structs.h"
#include <string.h>
#include <stdio.h>
#include "board_piece.h"
#include <stdlib.h>
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
     print_board_copy(toPrint);
     printf("\nRendered:\n\n");
     print_board(toPrint);
}

void freeBLL(boardLinkedList *toFree){
     if(toFree->next != 0)
     {
          freeBLL(toFree->next);
     }
     free(toFree);
     return;
}

void freeCLL(coordLL *toFree){
     if(toFree->next != 0)
     {
          freeCLL(toFree->next);
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
coordLL* appendToCLL(int x, int y,coordLL *head)
{
     
     head->x = x;
     head->y = y;
     head->next = (coordLL *)malloc(sizeof(coordLL));
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
     print_board(root->current);
     printBLL(root->next);


}