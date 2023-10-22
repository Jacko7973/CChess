/* chessutils.h */
/* Jack O'Connor 2023 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* Macros */

#ifndef BOARD_DIM
#define BOARD_DIM (8)
#endif
#define PIECE_TYPE_STR (" prnbqk")

/* Piece Type bitmasks */

#define PIECE_TYPE_BM   (0x7)
#define PIECE_COLOR_BM  (0x8)


/* Custom types */

typedef uint8_t Piece;

typedef enum {
    EMPTY,
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING,

    WHITE = 8,
    BLACK = 16,
    INVALID = 24,
} PieceType;


typedef struct {
    Piece *data;
    uint8_t turn;
    size_t halfmove_clock;
    size_t fullmove_number;
} Board;

/* Funtion headers */

Piece       piece_get_type(Piece p);
Piece       piece_get_color(Piece p);


Board *     board_create(const char *fen);
void        board_delete(Board *b);

Piece *     board_at(Board *b, size_t x, size_t y);

