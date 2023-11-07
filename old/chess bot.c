

#pragma region // Includes, defines and globals

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>      /* wint_t */
#include <locale.h>
#define ARR_SIZE(arr) ( sizeof((arr)) / sizeof((arr[0])) )
#define BOARD_SIZE 8
#define DEPTH 3
#define COMPUTER_IS_WHITE false
// For final
/*
#define INITIAL_BOARD_STATE          \
     {                               \
          {7, 3, 5, -1, 15, -1, 3, -1}, \
          {-1, 1, 1, 1, -1, -1, -1, -1}, \
          {-1, -1, -1, -1, 1, -1, 1, 7}, \
          {-1, 1, -1, 0, -1, 1, -1, -1}, \
          {-1, -1, 14, -1, -1, 5, -1, 0}, \
          {-1, 6, -1, -1, -1, -1, -1, -1}, \
          {0, -1, 0, -1,9, -1, -1, -1} , \
          {-1, -1, -1, -1, -1, 2, -1, -1}  \
     }
//*/

#define INITIAL_BOARD_STATE          \
     {                               \
          { 17, 3, 5, 9,15, 5, 3, 17}, \
          { 1, 1, 1, 1, 1, 1, 1, 1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          { 0, 0, 0, 0, 0, 0, 0, 0}, \
          { 16, 2, 4, 8,14, 4, 2, 16}  \
     }
//*/
/*
#define INITIAL_BOARD_STATE          \
     {                               \
          {17, 3, 5, 9,15,-1,-1,17}, \
          { 1, 1, 1, 1, 1, 1, 1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1, -1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1, 0, 0,-1,-1, 1}, \
          {-1,-1, 2,-1,-1, 2,-1,-1}, \
          { 0, 0, 0,-1,-1,-1, 0, 0}, \
          {16,-1, 4,8,14, 4,-1, 16}  \
     }
/* 

#define INITIAL_BOARD_STATE          \
     {                               \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1, 6,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}, \
          {-1,-1,-1,-1,-1,-1,-1,-1}  \
     }
*/
typedef enum { false, true } bool;

/*Notes:
-1000 < eval < 1000
-eval = good for human 
+eval = good for bot

Piece indexes:
-1 = Nothing
White:      |  Black:
0 = Pawn    |  1 = Pawn
2 = Knight  |  3 = Knight
4 = Bishop  |  5 = Bishop
6 = Rook    |  7 = Rook
8 = Queen   |  9 = Queen
10 = King   |  11 = King
12 = PawnM2 |  13 = PawnM2
14 = UKing  |  15 = UKing
16 = URook  |  17 = URook

So GET_TYPE(piece) returns:
0 pawn
1 knight
2 Bishop
3 Rook
4 Queen
5 King
6 PawnM2
7 UKing
8 URook


UKing is an unmoved king (that can castle)
URook is an unmoved rook
PawnM2 is a pawn that has just moved two spaces forward, to allow for en passante

Black is up,
White is down
Boards are [UP->DOWN = 0 - 7][LEFT-RIGHT = 0-7]

*/
#define FORWARD(piece) ((piece & 1)*2-1)
#define IS_BLACK(piece) (piece & 1)
#define GET_TYPE(piece) (piece >> 1)
#define IS_EMPTY(piece) (piece == -1)
#define PAWN 0
#define KNIGHT 1
#define BISHOP 2
#define ROOK 3
#define QUEEN 4
#define KING 5
#define PAWN_M2 6
#define U_KING 7
#define U_ROOK 8
#define NUM_PIECE_TYPES 6
const char INPUT_PIECE_CHARS[6] = {'p','k','b','r','Q','K'};
const char x_chars[8] = {'a','b','c','d','e','f','g','h'};
const char y_chars[8] = {'8','7','6','5','4','3','2','1'};

//Flipped for different terminals: 
const int PIECE_CHARS[19] = {0x0020,0x265F,0x2659, 0x265E,0x2658, 0x265D,0x2657, 0x265C,0x2656, 0x265B,0x2655, 0x265A,0x2654, 0x265F,0x2659, 0x265A,0x2654, 0x265C,0x2656};
//const int pieceChars[19] = {0x0020,0x2659,0x265F, 0x2658,0x265E, 0x2657, 0x265D, 0x2656,0x265C, 0x2655,0x265B, 0x2654,0x265A, 0x2659,0x265F, 0x2654,0x265A, 0x2656,0x265C};

const int POINT_VALUES[9] = {1, 3, 3, 5, 9,1000,1,1000,5};
#pragma endregion

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
                    printf("\x1b[40m %lc ", (wint_t)PIECE_CHARS[toPrint[y][x]+1]);
               }else{
                    printf("\x1b[41m %lc ", (wint_t)PIECE_CHARS[toPrint[y][x]+1]);
               }
          }
          printf("\x1b[40m %d\n",8-y);
     }
     printf("   a  b  c  d  e  f  g  h \n");

}

void printBoardCopy (board toPrint){
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
          printf(" }, \\ \n",8-y);
     }
     printf("\n\n");

}


typedef struct boardPosition
{
     bool blackMove;
     int eval;
     board position;
     struct boardPosition *children;
} boardPosition;

typedef struct boardLinkedList
{
     board current;
     struct boardLinkedList *next;
} boardLinkedList;

boardLinkedList *HISTORY;
boardLinkedList *HISTORY_HEAD;

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
     printBoardCopy(toPrint);
     printf("\nRendered:\n\n");
     printBoard(toPrint);
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

boardLinkedList* movePiece(board newBoard,boardLinkedList *head, piece king){
     if(inCheck(newBoard,king))
     {
          //printf("check ");
          return head;
     }
     return appendToBLL(newBoard, head);
}


#pragma endregion

#pragma region // Possible Move Processing
// This is a group of functions that define how the pieces move (promotion and en passante not yet implemented)





#pragma endregion

#pragma region // Computer Move Choosing

int eval_board(board to_eval){
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
               if(GET_COLOR(piece) == COMPUTER_COLOR){
                    eval -= POINT_VALUES[GET_TYPE(piece)];
               }else{
                    eval += POINT_VALUES[GET_TYPE(piece)];

               }
          }
     }
     return eval;
}
int minimax_odd(int depth, board current);
int minimax_even(int depth, board current){
     if(depth == 0){
          //printf("Evaled board at %d\n",eval_board(current));
          return eval_board(current);
     }
     boardLinkedList *possibleMoves = getPossibleMovesFromBoard(current,COMPUTER_COLOR);
     boardLinkedList *head = possibleMoves;
     int min =  100000;
     while(head->next != 0){
          int eval = minimax_odd(depth-1,head->current);
          if(eval < min)
               min = eval;
          head = head->next;
     }
     freeBLL(possibleMoves);
     return min;
}

int minimax_odd(int depth, board current){
     if(depth == 0){
          //printf("Evaled board at %d\n",eval_board(current));
          return eval_board(current);
     }
     boardLinkedList *possibleMoves = getPossibleMovesFromBoard(current,!COMPUTER_COLOR);
     boardLinkedList *head = possibleMoves;
     //printf("start minimax with depth %d and h->n = %d\n",depth,COMPUTER_COLOR);
     int max = -100000;
     
     while(head->next != 0){
          int eval = minimax_even(depth-1,head->current);
          if(eval > max)
               max = eval;
          head = head->next;
     }
     freeBLL(possibleMoves);
     return max;
}



int computer_move(board current){
     boardPosition state = {.blackMove = !COMPUTER_COLOR, .eval = 0x0};
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
          int eval = minimax_even((DEPTH-1)*2,to_explore);
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

#pragma endregion


#pragma region // Human Input Processing

piece read_piece(){
     char p, x, y;
     piece ret;
     printf("Make your move: ");
     scanf("\n%c%c%c",&p,&x,&y);
     //printf("Received p: %c, x: %c, y: %c\n",p,x,y);
     for(int i = 0;i<NUM_PIECE_TYPES;i++){
          if(INPUT_PIECE_CHARS[i] == p){
               ret.type = i;
          }
     }
     for(int i = 0;i<BOARD_SIZE;i++){
          if(x_chars[i] == x){
               ret.x = i;
          }
     }
     for(int i = 0;i<BOARD_SIZE;i++){
          if(y_chars[i] == y){
               ret.y = i;
          }
     }
     //printf("Moved to: %d, x: %d, y: %d\n",ret.type,ret.x,ret.y);
     
     return ret;
}


int human_move(board current){
     boardLinkedList *possibleMoves = getPossibleMovesFromBoard(current,COMPUTER_COLOR);
     if(possibleMoves->next == 0){
          freeBLL(possibleMoves);
          return 1;
     }
     while(true){ // repeat so players can retry if they make an illegal move
          boardLinkedList *head = possibleMoves;
          piece to_move = read_piece();
          if(to_move.x < 0 || to_move.x >= BOARD_SIZE || to_move.y < 0 || to_move.y >= 8 || to_move.type < 0 || to_move.type >= NUM_PIECE_TYPES || (GET_TYPE(current[to_move.y][to_move.x]) == to_move.type && GET_COLOR(current[to_move.y][to_move.x]) == COMPUTER_COLOR)){
               printf("Error: illegal move - reading\n");
               continue;
          }
          while(head->next != 0){
               int piece_to_check = head->current[to_move.y][to_move.x];
               if(
                    GET_COLOR(piece_to_check) == COMPUTER_COLOR
                    &&
                    (
                         GET_TYPE(piece_to_check) == to_move.type 
                         || 
                         (GET_TYPE(piece_to_check) == PAWN && to_move.type == PAWN_M2) 
                         || 
                         (GET_TYPE(piece_to_check) == PAWN_M2 && to_move.type == PAWN) 
                         || 
                         (GET_TYPE(piece_to_check) == ROOK && to_move.type == U_ROOK) 
                         || 
                         (GET_TYPE(piece_to_check) == KING && to_move.type == U_KING)
                    )
               )
               {
                    memcpy(current,head->current,sizeof(board));
                    freeBLL(possibleMoves);
                    return 0;
               }else{
                    printf("%d %d %d\n",piece_to_check,GET_TYPE(piece_to_check), to_move.type);
               }
               head = head->next;
          }
          if(head->next == 0){
               printf("Error: illegal move - generating\n");
          }
     }
     freeBLL(possibleMoves);
     return 0;
}

#pragma endregion


int main()
{
     setlocale(LC_ALL, "");     
     srand(time(NULL));
     //printBLL(initialMoves);
     board current = INITIAL_BOARD_STATE;
     HISTORY = (boardLinkedList *) malloc(sizeof(boardLinkedList));
     HISTORY_HEAD = HISTORY;
     HISTORY_HEAD = appendToBLL(current,HISTORY_HEAD);
     if(!COMPUTER_COLOR){
          printBoard(current);
          human_move(current);
          printBoard(current);
          HISTORY_HEAD = appendToBLL(current,HISTORY_HEAD);
          //sleep(1);

     }
     while(true){
          if(computer_move(current)){
               piece king = findPiece(current, KING*2+1-COMPUTER_COLOR);
               if(IS_EMPTY(king.type)){
                    king = findPiece(current, U_KING*2+1-COMPUTER_COLOR);
               }
               if(inCheck(current,king))
                    printf("Checkmate - YOU WIN!!!\n");
               else
                    printf("Stalemate - Draw\n");
               break;
          }
          HISTORY_HEAD = appendToBLL(current,HISTORY_HEAD);
          printBoard(current);
          if(human_move(current)){
               piece king = findPiece(current, KING*2+COMPUTER_COLOR);
               if(IS_EMPTY(king.type)){
                    king = findPiece(current, U_KING*2+COMPUTER_COLOR);
               }
               if(inCheck(current,king))
                    printf("Checkmate - you lose :(\n");
               else
                    printf("Stalemate - Draw\n");
               break;
          }
          HISTORY_HEAD = appendToBLL(current,HISTORY_HEAD);
          printBoard(current);
          //sleep(1);
     }


     printf("Final Position: \n");
     printBoard(current);
     freeBLL(HISTORY);
     //printf("\n%d\n",inCheck(current,findPiece(current,U_KING*2+1)));
     return 0;
}