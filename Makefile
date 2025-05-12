CC=			gcc
CFLAGS=		-Wall -std=gnu99 -g -Iinclude -fPIC -O3
LD=			gcc
LDFLAGS=	-Llib -Iinclude
LOAD=		LD_LIBRARY_PATH=lib/
LIBS=		-lchess
LIBSNG=		-lchess_ng

TARGETS=	bin/chess bin/chess_ng bin/unit_chess


ALL:	$(TARGETS)

bin/chess:			bin/chess.o	lib/libchess.so
	$(LD) $(LDFLAGS) $(LIBS) -o $@ $^

bin/chess_ng:		bin/chess_ng.o	lib/libchess_ng.so
	$(LD) $(LDFLAGS) $(LIBSNG) -o $@ $^

lib/libchess.so:	bin/list.o bin/bitboard.o bin/chessboard.o bin/chessgame.o
	$(LD) $(LDFLAGS) -shared -o $@ $^

lib/libchess_ng.so:	bin/bitboard.o bin/chessboard_ng.o
	$(LD) $(LDFLAGS) -shared -o $@ $^

bin/%.o:			src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

bin/unit_chess:		tests/unit_chess.c lib/libchess.so
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

run:	bin/chess_ng
	$(LOAD) $<

debug:	bin/chess
	$(LOAD) gdb $<

valgrind:	bin/chess
	$(LOAD) valgrind --leak-check=full $<

test:	bin/unit_chess
	$(LOAD) $<

clean:
	@rm $(TARGETS) bin/*.o lib/*.so

