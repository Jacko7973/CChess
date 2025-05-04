/* libchess
 * Jack O'Connor 2025
 * src/chessgame.c
 */

#include <stdlib.h>
#include <string.h>

#include "chess.h"

#include "chessboard.h"
#include "bitboard.h"
#include "list.h"


/* External Functions */

ChessGame *     chessgame_create(const char *fen, char *event, char *site, char *date, char *time, size_t round, char *white, char *black) {

    ChessGame *cg = (ChessGame *) calloc(1, sizeof(ChessGame));
    if (cg) {
        cg->board = chessboard_create(fen);
        cg->ply_count = 0;

        cg->event = event ? strdup(event) : NULL;
        cg->site = site ? strdup(site) : NULL;
        cg->date = date ? strdup(date) : NULL;
        cg->time = time ? strdup(time) : NULL;
        cg->round = round;
        cg->white = white ? strdup(white) : white;
        cg->black = black ? strdup(black) : black;
        cg->result = NULL;

        cg->moves = list_create();
    }
    return cg;
}


void            chessgame_delete(ChessGame *cg) {
    chessboard_delete(cg->board);
    free(cg->event);
    free(cg->site);
    free(cg->date);
    free(cg->time);
    free(cg->white);
    free(cg->black);
    free(cg->result);
    list_delete(cg->moves, false);
    free(cg);
}


bool        chessgame_move_pgn(ChessGame *cg, const char *pgn) {

    if (cg->result) return false;

    uint8_t color = (cg->ply_count % 2 == 0) ? WHITE : BLACK;
    ChessMove move = chessboard_move_from_pgn(cg->board, pgn, color);

    if (move == (ChessMove)-1) return false;

    if (!chessboard_move(cg->board, move)) return false;

    list_append(cg->moves, (void *)move);
    cg->ply_count++;

    uint8_t enemy_color = (color == WHITE) ? BLACK : WHITE;
    Bitboard enemy_targets = 0L;
    chessboard_all_target_positions(cg->board, enemy_color, &enemy_targets, true);

    if (enemy_targets == 0L) {
        if (chessboard_in_check(cg->board, enemy_color)) {
            if (color == WHITE) cg->result = strdup("1-0");
            else cg->result = strdup("0-1");
        } else {
            cg->result = strdup("1/2 - 1/2");
        }
    }

    return true;
}

bool            chessgame_to_fen(ChessGame *cg, char *fen) {
    
    strcpy(fen, "");
    char board_fen[128];
    if (!chessboard_to_fen(cg->board, board_fen)) return false;
    strcat(fen, board_fen);
    strcat(fen, " ");

    char side_to_move[2] = { 0 };
    side_to_move[0] = ((cg->ply_count % 2) == 0) ? 'w' : 'b';
    strcat(fen, side_to_move);
    strcat(fen, " ");

    char castle_avail[5] = { 0 };

    return true;
}


