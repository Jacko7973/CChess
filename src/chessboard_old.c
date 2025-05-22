/* libchess
 * Jack O'Connor 2025
 * src/chessboard.c
 */

#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "chessboard.h"
#include "bitboard.h"


/* Internal Constants */

const char *    PIECE_CHARS = "pnbrqk";

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

const uint64_t DIAGONAL_LR = 0x8040201008040201;
const uint64_t DIAGONAL_RL = 0x0102040810204080;


/* Internal Functions */

char            get_piece_char(ChessPiece piece) {
    
    piece = (piece & PIECE_TYPE_BITMASK) >> 1;
    for (int i = 0; i < 6; i++) {
        if (piece & 1) return PIECE_CHARS[i];
        piece = piece >> 1;
    }

    return '\0';
}

ChessPiece      get_char_piece(char c) {

    c = tolower(c);
    ChessPiece piece = PAWN;
    for (char *cmp = (char *)PIECE_CHARS; *cmp; cmp++) {
        if (c == *cmp) return piece;
        piece = piece << 1;
    }

    return (ChessPiece)0L;
}


/* External Functions */

Chessboard *    chessboard_create(const char *fen) {
    
    Chessboard *cb = (Chessboard *) calloc(1, sizeof(Chessboard));
    if (cb) {
        cb->enpassant_target = -1;

        if (!fen) {
            // Default starting position
            cb->data[0] = WHITE | ROOK;
            cb->data[1] = WHITE | KNIGHT;
            cb->data[2] = WHITE | BISHOP;
            cb->data[3] = WHITE | QUEEN;
            cb->data[4] = WHITE | KING;
            cb->data[5] = WHITE | BISHOP;
            cb->data[6] = WHITE | KNIGHT;
            cb->data[7] = WHITE | ROOK;
            memset(cb->data + 8, WHITE | PAWN, 8);

            memset(cb->data + 48, BLACK | PAWN, 8);
            cb->data[56] = BLACK | ROOK;
            cb->data[57] = BLACK | KNIGHT;
            cb->data[58] = BLACK | BISHOP;
            cb->data[59] = BLACK | QUEEN;
            cb->data[60] = BLACK | KING;
            cb->data[61] = BLACK | BISHOP;
            cb->data[62] = BLACK | KNIGHT;
            cb->data[63] = BLACK | ROOK;
        } else {
            // Non-standard starting position
            char *c = (char *) fen;
            uint8_t file = 0, rank = 7;
            while (*c && *c != ' ') {
                
                if (isalpha(*c)) {
                    uint8_t color = (isupper(*c)) ? WHITE : BLACK;
                    ChessPiece piece = get_char_piece(*c);
                    if (piece) cb->data[(rank * 8) + file] = piece | color;
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
        }
    }

    return cb;
}


void            chessboard_delete(Chessboard *cb) {
    free(cb);
}


void            chessboard_dump(Chessboard *cb, FILE *stream) {

    for (ssize_t rank = 7; rank >= 0; rank--) {
        fprintf(stream, "+-----+-----+-----+-----+-----+-----+-----+-----+\n");
        fprintf(stream, "|     |     |     |     |     |     |     |     |\n");
        for (ssize_t file = 0; file < 8; file++) {
            ChessPiece piece = cb->data[rank * 8 + file];
            char piece_char;
            if (piece) {
                piece_char = get_piece_char(piece);
                piece_char = ((piece & PIECE_COLOR_BITMASK) == BLACK) ? piece_char : toupper(piece_char);
            } else {
                piece_char = ' ';
            }
            
            fprintf(stream, "|  %c  ", piece_char);
        }
        fprintf(stream, "|\n");
        fprintf(stream, "|     |     |     |     |     |     |     |     |\n");
    }
    fprintf(stream, "+-----+-----+-----+-----+-----+-----+-----+-----+\n");

}


bool            chessboard_to_fen(Chessboard *cb, char *out) {

    strcpy(out, "");

    for (int8_t rank = 7; rank >= 0; rank--) {
        char rank_buf[16] = { 0 };
        char *c = rank_buf;
        int empty = 0;
        for (int8_t file = 0; file < 8; file++) {
            ChessPiece *piece = chessboard_get(cb, file, rank);
            if (!piece) return false;
            else if (!*piece) {
                empty++;
                continue;
            } else {
                if (empty) {
                    *(c++) = '0' + empty;
                    empty = 0;
                }

                char piece_char = get_piece_char(*piece);
                if ((*piece & PIECE_COLOR_BITMASK) == WHITE) piece_char = toupper(piece_char);
                *(c++) = piece_char;
            }
        }
        if (empty) {
            *(c++) = '0' + empty;
            empty = 0;
        }

        if (rank != 0) *(c++) = '/';
        strcat(out, rank_buf);
    }

    return true;
}


ChessPiece *    chessboard_get(Chessboard *cb, uint8_t file, uint8_t rank) {

    if (rank >= 8 || file >= 8) return NULL;

    return (ChessPiece *)&cb->data + (rank * 8) + file;

}


Chessboard *    chessboard_duplicate(Chessboard *cb) {

    Chessboard *new = chessboard_create(NULL);
    memcpy(new->data, cb->data, 64 * sizeof(ChessPiece));
    return new;
}


bool            chessboard_verify_move(Chessboard *cb, ChessMove move) {

    uint8_t from    = ((uint16_t)move & MOVE_FROM_BITMASK);
    uint8_t to      = ((uint16_t)move & MOVE_TO_BITMASK) >> 6;
    // uint16_t flags  = ((uint16_t)move & MOVE_FLAG_BITMASK);

    if (from >= 64 || to >= 64) return false;

    // uint8_t file_from = from / 8, rank_from = from % 8;
    // uint8_t file_to = to / 8, rank_to = to % 8;
    
    ChessPiece piece_from = cb->data[from];
    uint8_t color_from = piece_from & PIECE_COLOR_BITMASK;
    uint8_t type_from = (piece_from & PIECE_TYPE_BITMASK) >> 1;
    if (!type_from) return false;

    ChessPiece piece_to = cb->data[to];
    uint8_t color_to = piece_to & PIECE_COLOR_BITMASK;
    uint8_t type_to = (piece_to & PIECE_TYPE_BITMASK) >> 1;
    if (type_to && color_from == color_to) return false;

    
    Chessboard *new = chessboard_duplicate(cb);
    chessboard_move(new, move);
    
    bool in_check = chessboard_in_check(new, color_from);

    chessboard_delete(new);
    return !in_check;
}


bool            chessboard_move(Chessboard *cb, ChessMove move) {

    // TODO: Check legality
    
    uint8_t from    = ((uint16_t)move & MOVE_FROM_BITMASK);
    uint8_t to      = ((uint16_t)move & MOVE_TO_BITMASK) >> 6;
    uint16_t flags  = ((uint16_t)move & MOVE_FLAG_BITMASK);

    ChessPiece move_piece = cb->data[from];
    uint8_t color = move_piece & PIECE_COLOR_BITMASK;
    uint8_t type = (move_piece & PIECE_TYPE_BITMASK);

    switch (flags) {
        case (P2R):
            move_piece = color | ROOK;
            break;
        case (P2N):
            move_piece = color | KNIGHT;
            break;
        case (P2B):
            move_piece = color | BISHOP;
            break;
        case (P2Q):
            move_piece = color | QUEEN;
            break;
        default:
            break;
    }

    cb->data[to] = MOVED | move_piece;
    cb->data[from] = 0;

    if (type == KING && (to - from) == 2) { // Short castle
        cb->data[from + 1] = MOVED | cb->data[from + 3];
        cb->data[from + 3] = 0;
    } else if (type == KING && (from - to) == 2) { // Long castle
        cb->data[from - 1] = MOVED | cb->data[from - 4];
        cb->data[from - 4] = 0;
    }

    // Process En Passant
    if (type == PAWN && to == cb->enpassant_target) {
        int8_t direction = (color == WHITE) ? 1 : -1;
        ChessPiece *captured_pawn = chessboard_get(cb, to % 8, (to / 8) - direction);

        *captured_pawn = 0;
    }

    // Reset En Passant target
    cb->enpassant_target = -1;
    if (type == PAWN) {
        if (color == WHITE && to - from == 16) {
            cb->enpassant_target = (to + from) / 2;
        } else if (color == BLACK && from - to == 16) {
            cb->enpassant_target = (to + from) / 2;
        }
    }

    return true;
}


bool            chessboard_in_check(Chessboard *cb, uint8_t color) {

    uint8_t enemy_color = (color == WHITE) ? BLACK : WHITE;
    Bitboard targets = 0L;
    chessboard_all_target_positions(cb, enemy_color, &targets, false);

    for (size_t i = 0; i < 64; i++) {
        ChessPiece piece = cb->data[i];
        if (!piece) continue;
        if ((piece & PIECE_COLOR_BITMASK) != color) continue;
        if ((piece & PIECE_TYPE_BITMASK) != KING) continue;

        if (!((targets >> i) & 1L)) break;

        return true;
    }

    return false;
}


ChessMove       chessmove_create(uint16_t position_from, uint16_t position_to, uint16_t flags) {

    ChessMove move = 0L;
    move |= (position_from & 0x3F);
    move |= (position_to & 0x3F) << 6;
    move |= flags;
    return move;

}


ChessMove       chessboard_move_from_pgn(Chessboard *cb, const char *move_pgn, ChessPiece color) {
    
    uint16_t file_to = -1, rank_to = -1;
    uint16_t file_from = -1, rank_from = -1;
    char piece_char = 'P';

    uint16_t move_flags = 0;

    // Castle
    if (!strcmp(move_pgn, "O-O")) {
        ChessPiece *king = chessboard_get(cb, 4, color * 7);
        if (!king || *king != (color | KING)) return -1L;
        Bitboard king_moves = 0L;
        chessboard_piece_target_positions(cb, (color * 56) + 4, &king_moves, true);
        if (!bitboard_get(&king_moves, 6, color * 7)) return -1L;
        return chessmove_create(4 + (color * 56), 6 + (color * 56), 0);
    } else if (!strcmp(move_pgn, "O-O-O")) {
        ChessPiece *king = chessboard_get(cb, 4, color * 7);
        if (!king || *king != (color | KING)) return -1L;
        Bitboard king_moves = 0L;
        chessboard_piece_target_positions(cb, (color * 56) + 4, &king_moves, true);
        if (!bitboard_get(&king_moves, 2, color * 7)) return -1L;
        return chessmove_create(4 + (color * 56), 2 + (color * 56), 0);
    }

    char *c = (char *)move_pgn + strlen(move_pgn) - 1;
    while (c >= move_pgn) {
        if (*c == '+') {
            // Check
        } else if (*c == '#') {
            // Mate
        } else if (*c == '=') {
            // Promotion
            piece_char = 'P';
            switch (*(c+1)) {
                case 'Q':
                    move_flags = P2Q;
                    break;
                case 'R':
                    move_flags = P2R;
                    break;
                case 'B':
                    move_flags = P2B;
                    break;
                case 'N':
                    move_flags = P2N;
                    break;
            }
        } else if (*c == 'x') {
            // Capture
        } else if (isalpha(*c)) {
            // File or piece identifier
            if ('a' <= *c && *c <= 'h') {
                // File
                if (file_to == (uint16_t)-1) file_to = (uint16_t)*c -'a';
                else if (file_from == (uint16_t)-1) file_from = (uint16_t)*c - 'a';
                else return -1L;
            } else {
                // Piece indicator
                piece_char = *c;
            }
        } else if (isdigit(*c)) {
            // Rank
            if (rank_to == (uint16_t)-1) rank_to = (uint16_t)*c - '1';
            else if (rank_from == -1) rank_from = (uint16_t)*c - '1';
            else return -1L;
        } else {
            // Unknwon character
            return -1L;
        }

        c--;
    }

    if (file_to >= 8 || rank_to >= 8) return -1L;

    uint16_t position_to = (rank_to * 8) + file_to;

    ChessPiece move_type = get_char_piece(piece_char);
    if (!move_type) return -1L;

    // Search for the moving piece
    ChessPiece *move_piece = NULL;
    uint16_t rank_lower = (rank_from != (uint16_t)-1) ? rank_from : 0;
    uint16_t rank_upper = (rank_from != (uint16_t)-1) ? rank_from : 7;
    uint16_t file_lower = (file_from != (uint16_t)-1) ? file_from : 0;
    uint16_t file_upper = (file_from != (uint16_t)-1) ? file_from : 7;
    for (uint16_t rank = rank_lower; rank <= rank_upper; rank++) {
        for (uint16_t file = file_lower; file <= file_upper; file++) {
            uint16_t position = (rank * 8) + file;
            ChessPiece *current_piece = &cb->data[position];
            if (!*current_piece) continue;
            if ((*current_piece & PIECE_COLOR_BITMASK) != color) continue;
            if ((*current_piece & PIECE_TYPE_BITMASK) != move_type) continue;

            // Check that current piece targets desired square
            Bitboard out = (Bitboard)0L;
            if (!chessboard_piece_target_positions(cb, position, &out, true)) continue;
            if (((out >> position_to) & 1L) == 0) continue;

            move_piece = current_piece;
            break;
        }

        if (move_piece) break;
    }

    if (!move_piece) return -1L;

    uint16_t position_from = move_piece - (ChessPiece *)&cb->data;

    return chessmove_create(position_from, position_to, move_flags);
}


bool            chessboard_piece_target_positions(Chessboard *cb, uint64_t position, Bitboard *out, bool stop_check) {

    *out = 0L;

    if (position >= 64) return false;

    ChessPiece piece = cb->data[position];
    uint8_t color = piece & PIECE_COLOR_BITMASK;
    uint8_t type = piece & PIECE_TYPE_BITMASK;
    bool moved = piece & MOVED;

    uint8_t rank = position / 8, file = position % 8;

    if (!type) return false;

    if (type == PAWN) {
        if (color == WHITE && rank == 7) return true;
        if (color == BLACK && rank == 0) return true;

        int8_t direction = (color == WHITE) ? 1 : -1;
        
        if (!*chessboard_get(cb, file, rank + direction)) {
            bitboard_set(out, file, rank + direction, true);

            if (!moved && !*chessboard_get(cb, file, rank + 2 * direction))
                bitboard_set(out, file, rank + 2 * direction, true);
        }


        ChessPiece *target_l = chessboard_get(cb, file - 1, rank + direction);
        ChessPiece *target_r = chessboard_get(cb, file + 1, rank + direction);
        if (target_l && *target_l && (*target_l & PIECE_COLOR_BITMASK) != color)
            bitboard_set(out, file - 1, rank + direction, true);
        if (target_r && *target_r && (*target_r & PIECE_COLOR_BITMASK) != color)
            bitboard_set(out, file + 1, rank + direction, true);

        // En Passant
        if (cb->enpassant_target != -1) {
            uint8_t ep_file = cb->enpassant_target % 8;
            uint8_t ep_rank = cb->enpassant_target / 8;

            if (rank + direction == ep_rank) {
                if (file - ep_file == 1 || ep_file - file == 1)
                    bitboard_set(out, ep_file, ep_rank, true);
            }
        }

    } else if (type == KNIGHT) {

        for (size_t i = 0; i < 8; i++) {
            uint8_t file_offset = KNIGHT_OFFSETS[i][0];
            uint8_t rank_offset = KNIGHT_OFFSETS[i][1];

            ChessPiece *target_piece = chessboard_get(cb, file + file_offset, rank + rank_offset);

            if (!target_piece) continue;
            if (*target_piece && (*target_piece & PIECE_COLOR_BITMASK) == color)
                continue;

            bitboard_set(out, file + file_offset, rank + rank_offset, true);
        }

    } else if (type == BISHOP) {

        for (size_t i = 0; i < 4; i++) {
            size_t direction_index = DIRECTIONS_BISHOP[i];
            int file_offset = DIRECTIONS[direction_index][0];
            int rank_offset = DIRECTIONS[direction_index][1];

            uint8_t new_file = file + file_offset, new_rank = rank + rank_offset;
            while (new_file < 8 && new_rank < 8) {
                ChessPiece *target = chessboard_get(cb, new_file, new_rank);
                if (target && *target && (*target & PIECE_COLOR_BITMASK) == color) break;
                bitboard_set(out, new_file, new_rank, true);
                if (target && *target) break;
                new_file += file_offset;
                new_rank += rank_offset;
            }
        }

    } else if (type == ROOK) {

        for (size_t i = 0; i < 4; i++) {
            size_t direction_index = DIRECTIONS_ROOK[i];
            int file_offset = DIRECTIONS[direction_index][0];
            int rank_offset = DIRECTIONS[direction_index][1];

            uint8_t new_file = file + file_offset, new_rank = rank + rank_offset;
            while (new_file < 8 && new_rank < 8) {
                ChessPiece *target = chessboard_get(cb, new_file, new_rank);
                if (target && *target && (*target & PIECE_COLOR_BITMASK) == color) break;
                bitboard_set(out, new_file, new_rank, true);
                if (target && *target) break;
                new_file += file_offset;
                new_rank += rank_offset;
            }
        }

    } else if (type == QUEEN) {

        for (size_t i = 0; i < 8; i++) {
            size_t direction_index = DIRECTIONS_QUEEN[i];
            int file_offset = DIRECTIONS[direction_index][0];
            int rank_offset = DIRECTIONS[direction_index][1];

            uint8_t new_file = file + file_offset, new_rank = rank + rank_offset;
            while (new_file < 8 && new_rank < 8) {
                ChessPiece *target = chessboard_get(cb, new_file, new_rank);
                if (target && *target && (*target & PIECE_COLOR_BITMASK) == color) break;
                bitboard_set(out, new_file, new_rank, true);
                if (target && *target) break;
                new_file += file_offset;
                new_rank += rank_offset;
            }
        }

    } else if (type == KING) {

        for (size_t i = 0; i < 8; i++) {
            int file_offset = DIRECTIONS[i][0];
            int rank_offset = DIRECTIONS[i][1];

            uint8_t new_file = file + file_offset, new_rank = rank + rank_offset;
            ChessPiece *target = chessboard_get(cb, new_file, new_rank);
            if (target && *target && (*target & PIECE_COLOR_BITMASK) == color) continue;
            bitboard_set(out, new_file, new_rank, true);
        }

        // TODO: Fix this =)
        // Castling
        if (stop_check) {
            Bitboard enemy_targets = 0L;
            chessboard_all_target_positions(cb, (color == WHITE) ? BLACK : WHITE, &enemy_targets, false);

            // Short castle
            ChessPiece *rook_r = chessboard_get(cb, 7, rank);
            if (
                    !moved &&
                    rook_r &&
                    *rook_r == (color | ROOK) &&
                    !cb->data[(rank * 8) + 5] &&
                    !cb->data[(rank * 8) + 6] &&
                    !bitboard_get(&enemy_targets, 5, rank) &&
                    !bitboard_get(&enemy_targets, 6, rank)
               ) {
                bitboard_set(out, 6, rank, true);
            }

            // Long castle
            ChessPiece *rook_l = chessboard_get(cb, 0, rank);
            if (
                    !moved &&
                    rook_l &&
                    *rook_l == (color | ROOK) &&
                    !cb->data[(rank * 8) + 1] &&
                    !cb->data[(rank * 8) + 2] &&
                    !cb->data[(rank * 8) + 3] &&
                    !bitboard_get(&enemy_targets, 2, rank) &&
                    !bitboard_get(&enemy_targets, 3, rank)
               ) {
                bitboard_set(out, 2, rank, true);
            }
        }


    } else  {
        return false;
    }

    if (!stop_check) return true;

    // Check for checking moves
    for (uint8_t rank = 0; rank < 8; rank++) {
        for (uint8_t file = 0; file < 8; file++) {
            if (!bitboard_get(out, file, rank)) continue;

            uint16_t position_to = (rank * 8) + file;
            ChessMove move = chessmove_create(position, position_to, 0L);
            if (chessboard_verify_move(cb, move)) continue;

            bitboard_set(out, file, rank, false);
        }
    }

    return true;
}


bool            chessboard_all_target_positions(Chessboard *cb, uint8_t color, Bitboard *out, bool stop_check) {

    for (uint8_t rank = 0; rank < 8; rank++) {
        for (uint8_t file = 0; file < 8; file++) {
            ChessPiece *piece = chessboard_get(cb, file, rank);
            if (!piece || !*piece) continue;
            if ((*piece & PIECE_COLOR_BITMASK) != color) continue;
            
            Bitboard new = 0L;
            chessboard_piece_target_positions(cb, (rank * 8) + file, &new, stop_check);
            *out |= new;
        }
    }

    return true;
}


size_t          chessboard_count_moves(Chessboard *cb, uint8_t color, size_t ply) {

    if (ply == 0) return 1;

    size_t total = 0;
    for (uint8_t rank = 0; rank < 8; rank++) {
        for (uint8_t file = 0; file < 8; file++) {
            ChessPiece *piece = chessboard_get(cb, file, rank);
            if (!piece || !*piece) continue;
            if ((*piece & PIECE_COLOR_BITMASK) != color) continue;

            Bitboard new = 0L;
            chessboard_piece_target_positions(cb, (rank * 8) + file, &new, true);
            if (!new) continue;

            for (size_t i = 0; i < 64; i++, new = new >> 1) {
                if (!(new & 1L)) continue;

                ChessMove move = chessmove_create((rank * 8) + file, i, 0);

                Chessboard *duplicate = chessboard_duplicate(cb);
                chessboard_move(duplicate, move);
                total += chessboard_count_moves(duplicate, (color == WHITE) ? BLACK : WHITE, ply - 1);
                chessboard_delete(duplicate);
            }
        }
    }

    return total;
}


