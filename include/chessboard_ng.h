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


#define MAX_MOVES   (218)
#define COLOR_ARR_INDEX(color)  ((color == WHITE) ? 0 : 1)
#define PIECE_ARR_INDEX(piece)  (piece & (PIECE_TYPE_BITMASK | PIECE_COLOR_BITMASK))

#define BB_IDX_ALL              (7)
#define BB_IDX_COLOR(color)     (color)
#define BB_IDX_PIECE(piece)     ((piece) & (PIECE_TYPE_BITMASK | PIECE_COLOR_BITMASK))

/* Types */

typedef struct {
    ChessPiece  board[64];
    size_t      halfmove_clock;
    size_t      fullmove_counter;

    Bitboard    locations[14];

    Bitboard    targets[14];

    uint8_t     to_move;
    int8_t      enpassant_target;
    uint8_t     castle_ability_w;
    uint8_t     castle_ability_b;

    uint8_t     king_pos_w;
    uint8_t     king_pos_b;
} ChessBoard;

typedef uint16_t ChessMove;


/* External Function */

ChessBoard *        chessboard_create(const char *fen);
void                chessboard_delete(ChessBoard *cb);
void                chessboard_dump(ChessBoard *cb, FILE *stream);
char *              chessboard_to_fen(ChessBoard *cb);

ChessPiece *        chessboard_get(ChessBoard *cb, uint8_t file, uint8_t rank);

bool                chessboard_make_move(ChessBoard *cb, ChessMove move);
bool                chessboard_unmake_move(ChessBoard *cb, ChessMove move);

size_t              chessboard_pseudolegal_moves(ChessBoard *board, ChessMove *out);

#endif

