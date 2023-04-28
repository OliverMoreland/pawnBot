
#pragma region // Computer Move Choosing

int computer_move(board current){
     boardPosition state = {.blackMove = !COMPUTER_IS_WHITE, .eval = 0x7000};
     memcpy(state.position,current,sizeof(board));
     boardLinkedList *possibleMoves = getPossibleMovesFromBoard(state.position,state.blackMove);
     if(possibleMoves->next == 0){
          //printf("0 comp moves generated");
          free(possibleMoves);
          return 1;
     }
     int index = rand() % BLL_length(possibleMoves);
     BLL_item(possibleMoves,index,current);

     freeBLL(possibleMoves);
     return 0;
}

#pragma endregion

