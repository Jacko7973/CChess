// Chess
// Jack O'Connor 2025
// chess.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chess.h"
#include "chessboard.h"


int main(int argc, char *argv[]) {

    // r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -
    ChessGame *cg = chessgame_create(NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL);
    // ChessGame *cg = chessgame_create("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R", NULL, NULL, NULL, NULL, 0, NULL, NULL);

    char buffer[BUFSIZ];
    while (!feof(stdin)) {
        chessboard_dump(cg->board, stdout);
        if (cg->result) {
            fprintf(stdout, "Result: %s\n", cg->result);
            break;
        }

        uint8_t turn = (cg->ply_count % 2 == 0) ? WHITE : BLACK;
        fprintf(stdout, "(%s)>>> ", (turn == WHITE) ? "WHITE" : "BLACK");
        
        if (!fgets(buffer, BUFSIZ, stdin)) break;
        buffer[strlen(buffer) - 1] = '\0';

        if (!strcmp(buffer, "count")) {
            fprintf(stdout, "%lu\n", chessboard_count_moves(cg->board, turn, 3));
        } else {
            chessgame_move_pgn(cg, buffer);
        }
    }

    fprintf(stdout, "\n");

    char fen[BUFSIZ];
    chessgame_to_fen(cg, fen);
    fputs(fen, stdout);

    chessgame_delete(cg);

    return EXIT_SUCCESS;
}

