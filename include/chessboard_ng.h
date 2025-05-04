// libchess
// Jack O'Connor 2025
// include/chessboard_ng.h

#ifndef CHESSBOARD_NG_H
#define CHESSBOARD_NG_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "chesspiece.h"
#include "bitboard.h"

/* Types */

typedef struct {
    ChessPiece  board[64];
    size_t      halfmove_clock;
    size_t      fullmove_counter;

    Bitboard    rook_targets;
    Bitboard    bishop_targets;
    Bitboard    queen_targets;

    uint8_t     to_move;
    int8_t      enpassant_target;
    uint8_t     castle_ability_w;
    uint8_t     castle_ability_b;

    uint8_t     king_pos_w;
    uint8_t     king_pos_b;
} ChessBoard;



/* External Function */

ChessBoard *        chessboard_create(const char *fen);
void                chessboard_delete(ChessBoard *cb);
void                chessboard_dump(ChessBoard *cb, FILE *stream);
char *              chessboard_to_fen(ChessBoard *cb);

ChessPiece *        chessboard_get(ChessBoard *cb, uint8_t file, uint8_t rank);
void                chessboard_update_sliding_targets(ChessBoard *cb, ChessPiece piece_type);

#endif

