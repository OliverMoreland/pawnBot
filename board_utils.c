#pragma region // Board, and BLL and Position Processing




typedef int board[BOARD_SIZE][BOARD_SIZE];

void printBoard (board toPrint){
     system("clear");
     printf("   a  b  c  d  e  f  g  h \n");
     for (int y = 0; y < BOARD_SIZE; y++)
     {
          printf("%d ",8-y);
          for (int x = 0; x < BOARD_SIZE; x++)
          {
               //printf("%d ",toPrint[y][x]+1);

               if(y&1 ^ x&1){
                    printf("\x1b[40m %lc ", (wint_t)pieceChars[toPrint[y][x]+1]);
               }else{
                    printf("\x1b[41m %lc ", (wint_t)pieceChars[toPrint[y][x]+1]);
               }
          }
          printf("\x1b[40m %d\n",8-y);
     }
     printf("   a  b  c  d  e  f  g  h \n");

}

typedef struct boardPosition
{
     bool blackMove;
     short eval;
     board position;
     struct boardPosition *children;
} boardPosition;

typedef struct boardLinkedList
{
     board current;
     struct boardLinkedList *next;
} boardLinkedList;

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

void freeBLL(boardLinkedList *toFree){
     if(toFree->next != 0)
     {
          freeBLL(toFree->next);
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

void printBLL(boardLinkedList *root){
     if(root->next == 0){
          printf("\nDONE\n");
          return;
     }
     printf("\n\n\nBoard\n\n");
     printBoard(root->current);
     printBLL(root->next);


}

typedef struct piece {
     int x;
     int y;
     int type;
} piece;

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
          (
          GET_TYPE(position[yToCheck][xToCheck]) != KNIGHT)
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

                    !(IS_BLACK(pieceToCheck) ^ IS_BLACK(king.type))
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

                    !(IS_BLACK(pieceToCheck) ^ IS_BLACK(king.type))
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

boardLinkedList* movePiece(board newBoard,boardLinkedList *head, piece king){
     if(inCheck(newBoard,king))
     {
          //printf("check ");
          return head;
     }
     return appendToBLL(newBoard, head);
}


#pragma endregion
