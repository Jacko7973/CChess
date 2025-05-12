// libchess
// Jack O'Connor 2025
// include/chesspiece.h

#ifndef CHESSPIECE_H
#define CHESSPIECE_H

#include <stdint.h>


/* Macros */

#define PIECE_TYPE_BITMASK      (0x07)
#define PIECE_COLOR_BITMASK     (0x08)
#define PIECE_CASTLE_BITMASK    (0x30)

#define piece_type(p)           (p & PIECE_TYPE_BITMASK)
#define piece_color(p)          (p & PIECE_COLOR_BITMASK)

/* Types */

typedef uint8_t ChessPiece;


/* Enums */

enum ChessPieceType { // First 3 bits of ChessPiece
    EMPTY       = 0,
    PAWN        = 1,
    KNIGHT      = 2,
    BISHOP      = 3,
    ROOK        = 4,
    QUEEN       = 5,
    KING        = 6
};

enum ChessPieceColor { // 4th bit of ChessPiece
    WHITE       = 0,
    BLACK       = 1<<3,
};

enum ChessPieceCastleAbility { // 5th and 6th bit of ChessPiece
    CAN_CASTLE_LONG     = 1<<4,
    CAN_CASTLE_SHORT    = 1<<5
};


#endif

