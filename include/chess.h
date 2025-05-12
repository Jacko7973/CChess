/* libchess
 * Jack O'Connor 2025
 * include/chess.h
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "list.h"
#include "bitboard.h"
#include "chessboard.h"


/* Types */

typedef struct {
    Chessboard *board;
    size_t ply_count;

    char *event;
    char *site;
    char *date;
    char *time;
    size_t round;
    char *white;
    char *black;
    char *result;

    List *moves;
} ChessGame;


/* Function Headers */

ChessGame *     chessgame_create(const char *fen, char *event, char *site, char *date, char *time, size_t round, char *white, char *black);
void            chessgame_delete(ChessGame *cg);

bool            chessgame_move_pgn(ChessGame *cg, const char *pgn);
bool            chessgame_to_fen(ChessGame *cg, char *fen);

