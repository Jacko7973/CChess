// libchess
// Jack O'Connor 2025
// src/chessboard_ng.c

#include <ctype.h>
#include <string.h>

#include "chessboard_ng.h"


/* Constants */

const Bitboard FILES[] = {
    0x0101010101010101L<<0,
    0x0101010101010101L<<1,
    0x0101010101010101L<<2,
    0x0101010101010101L<<3,
    0x0101010101010101L<<4,
    0x0101010101010101L<<5,
    0x0101010101010101L<<6,
    0x0101010101010101L<<7,
};

const Bitboard RANKS[] = {
    0x00000000000000FFL,
    0x000000000000FF00L,
    0x0000000000FF0000L,
    0x00000000FF000000L,
    0x000000FF00000000L,
    0x0000FF0000000000L,
    0x00FF000000000000L,
    0xFF00000000000000L,
};

const char *DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

const char *PIECE_CHARS = "pnbrqk";

const int KNIGHT_OFFSETS[][2] = {
    {-1, 2}, {1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}
};

const int DIRECTIONS[][2] = {
    {-1, -1}, {0, -1}, {1, -1},
    {-1,  0},          {1,  0},
    {-1,  1}, {0,  1}, {1,  1}
};

const size_t DIRECTIONS_BISHOP[] = {0, 2, 5, 7};
const size_t DIRECTIONS_ROOK[]   = {1, 3, 4, 6};
const size_t DIRECTIONS_QUEEN[]  = {0, 1, 2, 3, 4, 5, 6, 7, 8};



/* Functions */

/**
 * Get the ASCII character associated with piece.
 *
 * @param   piece   ChessPiece to convert to character.
 * @return  Character ASCII piece or 0 if error.
**/
char            get_piece_char(ChessPiece piece) {
    
    uint8_t color = piece & PIECE_COLOR_BITMASK;
    piece = piece & PIECE_TYPE_BITMASK;
    for (int i = 0; i < 6; i++) {
        if (piece == PAWN) {
            if (color == WHITE) return toupper(PIECE_CHARS[i]);
            else return PIECE_CHARS[i];
        }
        piece--;
    }

    return '\0';
}

/**
 * Get the ChessPiece associated with ASCII character.
 *
 * @param   c   ASCII character to convert to piece.
 * @return  ChessPiece or 0 if error.
**/
ChessPiece      get_char_piece(char c) {

    uint8_t color = (isupper(c)) ? WHITE : BLACK;
    c = tolower(c);
    ChessPiece piece = PAWN;
    for (char *cmp = (char *)PIECE_CHARS; *cmp; cmp++) {
        if (c == *cmp) return piece | color;
        piece++;
    }

    return EMPTY;
}

/**
 * Create ChessBoard structure.
 *
 * @param   fen     Forsyth-Edwards Notation string representing position (if NULL, default initial position is created).
 *
 * @return  Pointer to new ChessBoard structure.
**/
ChessBoard *        chessboard_create(const char *fen) {
    
    // Handle event that fen is NULL.
    if (!fen) return chessboard_create(DEFAULT_FEN);

    ChessBoard *cb = (ChessBoard *) calloc(1, sizeof(ChessBoard));
    if (cb) {
        cb->to_move = WHITE;
        cb->enpassant_target = -1;
        cb->halfmove_clock = 0;
        cb->fullmove_counter = 1;
        
        // Process position string
        char *c = (char *) fen;
        uint8_t file = 0, rank = 7;
        while (*c && *c != ' ') {
            
            if (isalpha(*c)) {
                ChessPiece piece = get_char_piece(*c);
                if (piece) {
                    uint8_t color = piece & PIECE_COLOR_BITMASK;
                    uint8_t type = piece & PIECE_TYPE_BITMASK;
                    if (type == KING) {
                        if (color == WHITE) cb->king_pos_w = (rank * 8) + file;
                        if (color == BLACK) cb->king_pos_b = (rank * 8) + file;
                    }

                    cb->board[(rank * 8) + file] = piece;
                }
            } else if (isdigit(*c)) {
                uint8_t skip = *c - '0';
                file += skip - 1;
            } else if (*c == '/') {
                file = 0;
                rank--;
                c++;
                continue;
            }

            file++;
            c++;
        }

        // Update sliding pice targets
        chessboard_update_sliding_targets(cb, BISHOP);
        chessboard_update_sliding_targets(cb, ROOK);
        chessboard_update_sliding_targets(cb, QUEEN);

        if (!*c) return cb;
        c++;

        // Process side-to-move
        if (*c == 'w') cb->to_move = WHITE;
        else if (*c == 'b') cb->to_move = BLACK;
        else return cb;

        if (!*c) return cb;
        c += 2;

        // Process castling ability
        while (*c && *c != ' ') {
            switch (*c) {
                case 'K':
                    cb->castle_ability_w |= CAN_CASTLE_SHORT;
                    break;
                case 'Q':
                    cb->castle_ability_w |= CAN_CASTLE_LONG;
                    break;
                case 'k':
                    cb->castle_ability_b |= CAN_CASTLE_SHORT;
                    break;
                case 'q':
                    cb->castle_ability_b |= CAN_CASTLE_LONG;
                    break;
                default:
                    return cb;
            }
            c++;
        }

        if (!*c) return cb;
        c++;

        // Process enpassant target square
        if (*c == '-') {
            cb->enpassant_target = -1;
        } else {
            uint8_t file = *(c++) - 'a';
            uint8_t rank = *c - '1';
            if (file >= 8 || rank >= 8) return cb;
            cb->enpassant_target = (rank * 8) + file;
        }

        if (!*c) return cb;
        c += 2;

        // Process move counters
        char counter_buf[64];
        strcpy(counter_buf, c);
        char *word = strtok(counter_buf, " ");
        if (!word) return cb;
        cb->halfmove_clock = atol(word);

        word = strtok(NULL, " ");
        if (!word) return cb;
        cb->fullmove_counter = atol(word);
    }
    return cb;
}


/**
 * Deallocate ChessBoard structure.
 *
 * @param   cb  Pointer to ChessBoard structure to delete.
**/
void                chessboard_delete(ChessBoard *cb) {

    free(cb);
}


void                chessboard_dump(ChessBoard *cb, FILE *stream) {

    for (ssize_t rank = 7; rank >= 0; rank--) {
        fprintf(stream, "  +---+---+---+---+---+---+---+---+\n");
        fprintf(stream, "%ld ", rank);
        for (ssize_t file = 0; file < 8; file++) {
            ChessPiece piece = cb->board[rank * 8 + file];
            char piece_char;
            if (piece) {
                piece_char = get_piece_char(piece);
            } else {
                piece_char = ' ';
            }
            
            fprintf(stream, "| %c ", piece_char);
        }
        fprintf(stream, "|\n");
    }
    fprintf(stream, "  +---+---+---+---+---+---+---+---+\n");
    fprintf(stream, "    a   b   c   d   e   f   g   h  \n");
}


/**
 * Get the FEN representation of a ChessBoard structure's position.
 *
 * @param   cb  Pointer to ChessBoard structure.
 *
 * @return  FEN string (must be freed) or NULL if error.
**/
char *              chessboard_to_fen(ChessBoard *cb) {

    char fen_buf[BUFSIZ] = { 0 };
    
    // Process piece position.
    char *c = fen_buf;
    int empty = 0;
    for (ssize_t rank = 7; rank >= 0; rank--) {
        for (ssize_t file = 0; file < 8; file++) {
            ChessPiece *piece = chessboard_get(cb, file, rank);
            if (!piece) return NULL;
            if (!*piece) {
                empty++;
                continue;
            }

            if (empty) {
                *(c++) = '0' + empty; 
                empty = 0;
            }
            char piece_char = get_piece_char(*piece);
            *(c++) = piece_char;
        }

        if (empty) {
            *(c++) = '0' + empty; 
            empty = 0;
        }
        if (rank) *(c++) = '/';
    }
    *(c++) = ' ';

    // Process side-to-move
    if (cb->to_move == WHITE) *(c++) = 'w';
    else *(c++) = 'b';
    *(c++) = ' ';

    // Process castle availability
    bool any_castle = false;
    if (cb->castle_ability_w & CAN_CASTLE_SHORT) {
        *(c++) = 'K';
        any_castle = true;
    }
    if (cb->castle_ability_w & CAN_CASTLE_LONG) {
        *(c++) = 'Q';
        any_castle = true;
    }
    if (cb->castle_ability_b & CAN_CASTLE_SHORT) {
        *(c++) = 'k';
        any_castle = true;
    }
    if (cb->castle_ability_b & CAN_CASTLE_LONG) {
        *(c++) = 'q';
        any_castle = true;
    }

    if (!any_castle) *(c++) = '-';
    *(c++) = ' ';

    // Process enpassant target square
    if (cb->enpassant_target == -1) {
        *(c++) = '-';
    } else {
        int8_t ep_file = cb->enpassant_target % 8;
        int8_t ep_rank = cb->enpassant_target / 8;
        *(c++) = 'a' + ep_file;
        *(c++) = '1' + ep_rank;
    }
    *(c++) = ' ';

    // Process move counters
    char counter_buf[64] = { 0 };
    snprintf(counter_buf, 64, "%lu %lu", cb->halfmove_clock, cb->fullmove_counter);
    strcat(fen_buf, counter_buf);

    return strdup(fen_buf);
}


/**
 * Access a piece from a ChessBoard structure
 *
 * @param   cb      Pointer to ChessBoard structure.
 * @param   file    File (0 indexed) of desired piece.
 * @param   rank    Rank (0 indexed) of desired piece.
 *
 * @return  Pointer to ChessPiece structure, or NULL if coordinates invalid.
**/
ChessPiece *        chessboard_get(ChessBoard *cb, uint8_t file, uint8_t rank) {

    if (file >= 8 || rank >= 8) return NULL;

    return cb->board + (rank * 8) + file;
}


void                chessboard_update_sliding_targets(ChessBoard *cb, ChessPiece piece_type) {
    // TODO
}

