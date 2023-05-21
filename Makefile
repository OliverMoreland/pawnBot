CC = gcc
CFLAGS = -fPIC -g
HD = headers/
OBJS = main.o bot.o input.o board_piece.o bll.o moves.o
TARGET = chessbot
DEPS = $(HD)structs.h $(HD)globals.h $(HD)defs.h $(HD)initial_board.h $(HD)macros.h
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c $(HD)bot.h $(HD)input.h $(HD)board_piece.h $(HD)bll.h $(HD)moves.h $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

bot.o: bot.c $(HD)bot.h $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

input.o: input.c $(HD)input.h $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

board_piece.o: board_piece.c $(HD)board_piece.h $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

bll.o: bll.c  $(HD)bll.h $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

moves.o: moves.c $(HD)moves.h $(HD)bll.h $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS)
