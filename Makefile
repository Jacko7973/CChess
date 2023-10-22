CC = 		gcc
CFLAGS = 	-Wall -std=gnu99 -g -Iinclude
LD = 		gcc
LDFLAGS = 	-Llib
AR = 		ar
ARFLAGS = 	rcs

all: bin/chess_test

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

lib/libchess.a: src/chessutils.o src/terminalchess.o src/bitboard.o
	$(AR) $(ARFLAGS) $@ $^

bin/chess_test: src/chess_test.o lib/libchess.a
	$(LD) $(LDFLAGS) -o $@ $^


clean:
	rm bin/chess_test src/*.o

