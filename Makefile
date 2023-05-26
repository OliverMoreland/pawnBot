CC = gcc
HD = headers/
CFLAGS = -fPIC -g -Iheaders
SRCDIR = src/
OBJDIR = obj/
LIBDIR = lib/
TARGET = $(LIBDIR)libchessbot.so  # Output shared library filename
OTARGET = chessbot
DEPS = $(HD)structs.h $(HD)globals.h $(HD)defs.h $(HD)initial_board.h $(HD)macros.h
SOURCES = $(SRCDIR)main.c $(SRCDIR)bot.c $(SRCDIR)input.c $(SRCDIR)board_piece.c $(SRCDIR)bll.c $(SRCDIR)moves.c
OBJS = $(patsubst $(SRCDIR)%.c,$(OBJDIR)%.o,$(SOURCES))

.PHONY: all clean

all: $(TARGET) $(OTARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -shared -o $@ $^

$(OTARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^


$(OBJDIR)%.o: $(SRCDIR)%.c $(DEPS)
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJDIR)
	rm -f $(TARGET)