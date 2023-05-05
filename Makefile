CC = gcc
CFLAGS = 
OBJS = main.o bot.o input.o board_piece.o bll.o moves.o
TARGET = chessbot
DEPS = structs.h globals.h defs.h initial_board.h macros.h
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c bot.h input.h board_piece.h bll.h moves.h $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

bot.o: bot.c bot.h $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

input.o: input.c input.h $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

board_piece.o: board_piece.c board_piece.h $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

bll.o: bll.c  bll.h $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

moves.o: moves.c moves.h $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS)
