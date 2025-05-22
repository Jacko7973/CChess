CC=			gcc
CFLAGS=		-Wall -std=gnu99 -g -Iinclude -fPIC -O3
LD=			gcc
LDFLAGS=	-Llib -Iinclude
LOAD=		LD_LIBRARY_PATH=lib/
LIBS=		-lchess

TARGETS=	bin/chess bin/unit_chess


ALL:	$(TARGETS)

bin/chess:			bin/chess.o	lib/libchess.so
	$(LD) $(LDFLAGS) $(LIBS) -o $@ $^

lib/libchess.so:	bin/list.o bin/bitboard.o bin/chessboard.o
	$(LD) $(LDFLAGS) -shared -o $@ $^

bin/%.o:			src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

bin/unit_chess:		tests/unit_chess.c lib/libchess.so
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

run:	bin/chess
	$(LOAD) $<

debug:	bin/chess
	$(LOAD) gdb $<

valgrind:	bin/chess
	$(LOAD) valgrind --leak-check=full $<

test:	bin/unit_chess
	$(LOAD) $<

clean:
	@rm $(TARGETS) bin/*.o lib/*.so

