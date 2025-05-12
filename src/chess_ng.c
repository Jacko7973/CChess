// libchess
// Jack O'Connor 2025
// src/chess_ng.c

#include <stdio.h>
#include <stdlib.h>

#include "chessboard_ng.h"


int move_cmpfunc(const void *a, const void *b) {
    ChessMove move_a = *(const ChessMove *)a & 0xFFFlu;
    ChessMove move_b = *(const ChessMove *)b & 0xFFFlu;

    return (int)(move_a - move_b);
}


int main(int argc, char *argv[]) {

    ChessBoard *cb = chessboard_create(NULL);
    // ChessBoard *cb = chessboard_create("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");

    size_t moves_count;
    ChessMove moves[MAX_MOVES];
    do {
        moves_count = chessboard_pseudolegal_moves(cb, moves);
        qsort(moves, moves_count, sizeof(ChessMove), move_cmpfunc);

        Bitboard locations = 0lu;
        Bitboard targets = 0lu;
        for (size_t i = 0; i < moves_count; i++) {
            locations |= 1lu << (moves[i] & 0x3F);
            targets |= 1lu << ((moves[i] >> 6) & 0x3F);
        }
        bitboard_dump(&locations, stderr);
        fprintf(stderr, "\n");
        bitboard_dump(&targets, stderr);


        chessboard_dump(cb, stdout);
        fprintf(stdout, "(%s)>>> ", (cb->to_move == WHITE) ? "WHITE" : "BLACK");

        char buf[BUFSIZ];
        if (!fgets(buf, BUFSIZ, stdin)) break;

        uint8_t file_from, rank_from, file_to, rank_to;
        if (sscanf(buf, "%c%hhu%c%hhu", &file_from, &rank_from, &file_to, &rank_to) != 4) {
            fprintf(stdout, "Invalid move string: %s\n", buf);
            continue;
        }
        file_from -= 'a';
        file_to -= 'a';
        rank_from -= 1;
        rank_to -= 1;

        ChessMove move = (file_from + rank_from * 8) | ((file_to + rank_to * 8) << 6);

        if (!bsearch(&move, moves, moves_count, sizeof(ChessMove), move_cmpfunc)) {
            fprintf(stdout, "Illegal move: %s\n", buf);
            continue;
        }

        if (!chessboard_make_move(cb, move)) {
            fprintf(stdout, "Illegal move: %s\n", buf);
            continue;
        }

    } while (!feof(stdin));

    char *fen = chessboard_to_fen(cb);
    puts(fen);
    free(fen);

    chessboard_delete(cb);

    return EXIT_SUCCESS;
}

