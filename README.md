# pawnBot
This is a chess bot that I am making as a project.
Current (ish) state is: basic minimax bot
Pawn Position: a3 - Version 1.0 Completed


## Notes:

-1000 < eval < 1000
-eval = good for human 
+eval = good for bot

Piece indexes:
-1 = Nothing

| White:      |  Black: |
| --- | --- |
| 0 = Pawn    |  1 = Pawn
| 2 = Knight  |  3 = Knight |
| 4 = Bishop  |  5 = Bishop |
| 6 = Rook    |  7 = Rook |
| 8 = Queen   |  9 = Queen |
| 10 = King   |  11 = King |
| 12 = PawnM2 |  13 = PawnM2 |
| 14 = UKing  |  15 = UKing |
| 16 = URook  |  17 = URook

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

