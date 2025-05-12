// libchess
// Jack O'Connor 2025
// src/chessboard_ng.c

#include <ctype.h>
#include <string.h>

#include "chessboard_ng.h"


/* Constants */

const Bitboard A_FILE = 0x0101010101010101;
const Bitboard RANK_1 = 0x00000000000000FF;

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
    
    ChessPiece color = piece_color(piece);
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

    ChessPiece color = (isupper(c)) ? WHITE : BLACK;
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
    if (!fen) fen = DEFAULT_FEN;

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
                    ChessPiece color = piece_color(piece);
                    ChessPiece type = piece_type(piece);
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

        if (!*c) goto FEN_COMPLETE;
        c++;

        // Process side-to-move
        if (*c == 'w') cb->to_move = WHITE;
        else if (*c == 'b') cb->to_move = BLACK;
        else return cb;

        if (!*c) goto FEN_COMPLETE;
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
                    goto FEN_COMPLETE;
            }
            c++;
        }

        if (!*c) goto FEN_COMPLETE;
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

        if (!*c) goto FEN_COMPLETE;
        c += 2;

        // Process move counters
        char counter_buf[64];
        strcpy(counter_buf, c);
        char *word = strtok(counter_buf, " ");
        if (!word) goto FEN_COMPLETE;
        cb->halfmove_clock = atol(word);

        word = strtok(NULL, " ");
        if (!word) goto FEN_COMPLETE;
        cb->fullmove_counter = atol(word);

FEN_COMPLETE:
        // Populate bitboards
        for (size_t rank = 0; rank < 8; rank++) {
            for (size_t file = 0; file < 8; file++) {
                ChessPiece *piece = chessboard_get(cb, file, rank);
                if (!piece || !*piece) continue;

                ChessPiece color = piece_color(*piece);
                // ChessPiece type = piece_type(*piece);

                bitboard_set(cb->locations[BB_IDX_ALL], file, rank, true);
                bitboard_set(cb->locations[BB_IDX_COLOR(color)], file, rank, true);
                bitboard_set(cb->locations[BB_IDX_PIECE(*piece)], file, rank, true);

                // TODO: Set target locations
            }
        }
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
        fprintf(stream, "%ld ", rank + 1);
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


bool                chessboard_make_move(ChessBoard *cb, ChessMove move) {
    
    uint16_t position_from  = move & 0x3F;
    uint16_t position_to    = (move >> 6) & 0x3F;

    ChessPiece color = cb->to_move;
    ChessPiece enemy_color = (color == WHITE) ? BLACK : WHITE;

    // TODO: Check legality
    ChessPiece *piece_from = cb->board + position_from;
    ChessPiece *piece_to = cb->board + position_to;

    if (piece_color(*piece_from) != color) return false;
    if (*piece_to && piece_color(*piece_to) != enemy_color) return false;

    Bitboard bb_from    = (1lu << position_from);
    Bitboard bb_to      = (1lu << position_to);

    cb->locations[BB_IDX_ALL]                   &= ~bb_from;
    cb->locations[BB_IDX_ALL]                   |= bb_to;

    cb->locations[BB_IDX_COLOR(color)]          &= ~bb_from;
    cb->locations[BB_IDX_COLOR(color)]          |= bb_to;
    cb->locations[BB_IDX_PIECE(*piece_from)]    &= ~bb_from;
    cb->locations[BB_IDX_PIECE(*piece_from)]    |= bb_to;

    if (*piece_to) {
        cb->locations[BB_IDX_COLOR(enemy_color)]    &= ~bb_to;
        cb->locations[BB_IDX_PIECE(*piece_to)]      &= ~bb_to;
    }
    // TODO: Update target bitboards

    *piece_to = *piece_from;
    *piece_from = 0;

    cb->to_move = (cb->to_move == WHITE) ? BLACK : WHITE;
    return true;
}


bool                chessboard_unmake_move(ChessBoard *cb, ChessMove move);


/**
 * Generate list of pseudolegal moves (may put the player in check)
 *
 * @param   cb      Pointer to ChessBoard structure.
 * @param   out     Pointer to array of ChessMoves to populate 
 *                      (218 elements, terminated with null move)
 *
 * @return  Pointer to ChessPiece structure, or NULL if coordinates invalid.
**/
size_t              chessboard_pseudolegal_moves(ChessBoard *cb, ChessMove *out) {

    size_t move_idx = 0;
    ChessPiece color = cb->to_move;
    ChessPiece enemy_color = (color == WHITE) ? BLACK : WHITE;

    for (size_t rank = 0; rank < 8; rank++) {
        for (size_t file = 0; file < 8; file++) {
            if (!bitboard_get(cb->locations[BB_IDX_COLOR(color)], file, rank)) continue;

            uint8_t position = file + rank * 8;
            ChessPiece *piece = chessboard_get(cb, file, rank);
            ChessPiece type = piece_type(*piece);

            switch (type) {
                case PAWN:
                    // TODO
                    break;
                case KNIGHT:

                    for (size_t i = 0; i < sizeof(KNIGHT_OFFSETS)/sizeof(KNIGHT_OFFSETS[0]); i++) {
                        ssize_t dx = KNIGHT_OFFSETS[i][1];
                        ssize_t dy = KNIGHT_OFFSETS[i][0];
                        size_t new_file = file + dx;
                        size_t new_rank = rank + dy;

                        if (new_file >= 8 || new_rank >= 8) continue;
                        if (bitboard_get(cb->locations[BB_IDX_COLOR(color)], new_file, new_rank)) continue;
                        out[move_idx++] = position | ((new_file + new_rank * 8) << 6);
                        
                    }

                    break;
                case BISHOP:

                    for (size_t i = 0; i < sizeof(DIRECTIONS_BISHOP)/sizeof(size_t); i++) {
                        size_t dir_index = DIRECTIONS_BISHOP[i];
                        ssize_t dx = DIRECTIONS[dir_index][1];
                        ssize_t dy = DIRECTIONS[dir_index][0];
                        size_t new_file = file + dx;
                        size_t new_rank = rank + dy;

                        while (new_file <= 8 && new_rank <= 8) {
                            if (bitboard_get(cb->locations[BB_IDX_COLOR(color)], new_file, new_rank)) break;
                            out[move_idx++] = position | ((new_file + new_rank * 8) << 6);

                            new_file += dx;
                            new_rank += dy;

                            if (bitboard_get(cb->locations[BB_IDX_COLOR(enemy_color)], new_file, new_rank)) break;
                        }
                    }

                    break;
                case ROOK:

                    for (size_t i = 0; i < sizeof(DIRECTIONS_ROOK)/sizeof(size_t); i++) {
                        size_t dir_index = DIRECTIONS_ROOK[i];
                        ssize_t dx = DIRECTIONS[dir_index][1];
                        ssize_t dy = DIRECTIONS[dir_index][0];
                        size_t new_file = file + dx;
                        size_t new_rank = rank + dy;

                        while (new_file <= 8 && new_rank <= 8) {
                            if (bitboard_get(cb->locations[BB_IDX_COLOR(color)], new_file, new_rank)) break;
                            out[move_idx++] = position | ((new_file + new_rank * 8) << 6);

                            new_file += dx;
                            new_rank += dy;

                            if (bitboard_get(cb->locations[BB_IDX_COLOR(enemy_color)], new_file, new_rank)) break;
                        }
                    }

                    break;
                case QUEEN:

                    for (size_t i = 0; i < sizeof(DIRECTIONS_QUEEN)/sizeof(size_t); i++) {
                        size_t dir_index = DIRECTIONS_QUEEN[i];
                        ssize_t dx = DIRECTIONS[dir_index][1];
                        ssize_t dy = DIRECTIONS[dir_index][0];
                        size_t new_file = file + dx;
                        size_t new_rank = rank + dy;

                        while (new_file <= 8 && new_rank <= 8) {
                            if (bitboard_get(cb->locations[BB_IDX_COLOR(color)], new_file, new_rank)) break;
                            out[move_idx++] = position | ((new_file + new_rank * 8) << 6);

                            new_file += dx;
                            new_rank += dy;

                            if (bitboard_get(cb->locations[BB_IDX_COLOR(enemy_color)], new_file, new_rank)) break;
                        }
                    }

                    break;
                case KING:

                    for (size_t i = 0; i < 8; i++) {
                        size_t new_file = file + DIRECTIONS[i][1];
                        size_t new_rank = rank + DIRECTIONS[i][0];

                        if (new_file >= 8 || new_rank >= 8) continue;
                        if (bitboard_get(cb->locations[BB_IDX_COLOR(color)], new_file, new_rank)) continue;
                        if (bitboard_get(cb->targets[BB_IDX_COLOR(enemy_color)], new_file, new_rank)) continue;

                        out[move_idx++] = position | ((new_file + new_rank * 8) << 6);
                    }

                    break;
                default:
                    out[0] = 0;
                    fprintf(stderr, "chessboard_pseudolegal_moves\n");
                    return 0;
            }
        }
    }

    out[move_idx] = 0;
    return move_idx;
}

