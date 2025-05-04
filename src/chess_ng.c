// libchess
// Jack O'Connor 2025
// src/chess_ng.c

#include <stdio.h>
#include <stdlib.h>

#include "chessboard_ng.h"


int main(int argc, char *argv[]) {

    // ChessBoard *cb = chessboard_create(NULL);
    ChessBoard *cb = chessboard_create("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");

    do {
        chessboard_dump(cb, stdout);
        fprintf(stdout, "(%s) >>>", (cb->to_move == WHITE) ? "WHITE" : "BLACK");


        char buf[BUFSIZ];
        if (!fgets(buf, BUFSIZ, stdin)) break;
      

    } while (!feof(stdin));

    char *fen = chessboard_to_fen(cb);
    puts(fen);
    free(fen);

    M_bitboard_set(cb->rook_targets, 0, 0, true);
    M_bitboard_set(cb->rook_targets, 0, 0, false);
    if (M_bitboard_get(cb->rook_targets, 0, 0))
        puts("AAHH");

    chessboard_delete(cb);

    return EXIT_SUCCESS;
}

