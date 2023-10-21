CC = 		gcc
CFLAGS = 	-Wall -std=gnu99 -g
LD = 		gcc
LDFLAGS = 	-L.
AR = 		ar
ARFLAGS = 	rcs

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<


bin/chess_test: src/chess_test.o src/chessutils.o src/terminalchess.o src/bitboard.o
	$(LD) $(LDFLAGS) -o $@ $^


clean:
	rm bin/chess_test src/*.o

