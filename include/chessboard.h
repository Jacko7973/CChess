/* libchess
 * Jack O'Connor 2025
 * include/chessboard.h
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "bitboard.h"


/* Macros */

#define PIECE_COLOR_BITMASK (0x01)
#define PIECE_TYPE_BITMASK  (0x7E)
#define PIECE_FLAG_BITMASK  (0x80)

#define MOVE_FROM_BITMASK   (0x003F)
#define MOVE_TO_BITMASK     (0x0FC0)
#define MOVE_FLAG_BITMASK   (0xF000)

#define MAXIMUM_MOVES       (56)


/* Constants */


/* Types */

typedef uint8_t ChessPiece;

typedef uint16_t ChessMove;

typedef struct {
    ChessPiece data[64];
    ssize_t enpassant_target;
} Chessboard;


/* NG Types */

typedef struct {
    ChessPiece  data[64];
    size_t      halfmove;
    size_t      fullmove;
    uint8_t     turn;
    uint8_t     castle_ability;
    int8_t      enpassant_target;
    uint8_t     king_pos_w;
    uint8_t     king_pos_b;
} Chessboard_NG;

typedef struct {
    uint8_t     from;
    uint8_t     to;

    ChessPiece  moving_piece;

    char        pgn[8];
} ChessMove_NG;


typedef struct {
    size_t  nodes;
    size_t  captures;
    size_t  enpassants;
    size_t  castles;
    size_t  promotions;
    size_t  checks;
    size_t  discovery_checks;
    size_t  double_checks;
    size_t  checkmates;
} PerftResult;


/* Enums */

enum ChessPieceColor {
    WHITE   = 0,
    BLACK   = 1
};

enum ChessPieceType {
    PAWN    = 1<<1,
    KNIGHT  = 1<<2,
    BISHOP  = 1<<3,
    ROOK    = 1<<4,
    QUEEN   = 1<<5,
    KING    = 1<<6
};

enum ChessPieceFlag {
    MOVED   = 1<<7
};

enum ChessMoveFlag {
    P2R         = 1<<12,
    P2N         = 1<<13,
    P2B         = 1<<14,
    P2Q         = 1<<15,
};


enum CastleAvailability {
    CASTLE_SHORT_W  = 1<<0,
    CASTLE_LONG_W   = 1<<1,
    CASTLE_SHORT_B  = 1<<2,
    CASTLE_LONG_B   = 1<<3,
};

/* NG Enums */

enum ChessPieceType_NG { // First 3 bits of ChessPiece
    EMPTY_NG    = 0,
    PAWN_NG     = 1,
    KNIGHT_NG   = 2,
    BISHOP_NG   = 3,
    ROOK_NG     = 4,
    QUEEN_NG    = 5,
    KING_NG     = 6
};

enum ChessPieceColor_NG { // 4th bit of ChessPiece
    WHITE_NG    = 0,
    BLACK_NG    = 1<<3,
};



/* Function Headers */

Chessboard *    chessboard_create(const char *fen);
void            chessboard_delete(Chessboard *cb);
void            chessboard_dump(Chessboard *cb, FILE *stream);
bool            chessboard_to_fen(Chessboard *cb, char *out);
ChessPiece *    chessboard_get(Chessboard *cb, uint8_t file, uint8_t rank);
Chessboard *    chessboard_duplicate(Chessboard *cb);

bool            chessboard_verify_move(Chessboard *cb, ChessMove move);
bool            chessboard_move(Chessboard *cb, ChessMove move);
bool            chessboard_in_check(Chessboard *cb, uint8_t color);


ChessMove       chessmove_create(uint16_t position_from, uint16_t position_to, uint16_t flags);
ChessMove       chessboard_move_from_pgn(Chessboard *cb, const char *move_pgn, ChessPiece color);

bool            chessboard_piece_target_positions(Chessboard *cb, uint64_t position, Bitboard *out, bool stop_check);
bool            chessboard_all_target_positions(Chessboard *cb, uint8_t color, Bitboard *out, bool stop_check);

size_t          chessboard_count_moves(Chessboard *cb, uint8_t color, size_t ply);


