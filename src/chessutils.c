/* chessutils.c */
/* Jack O'Connor 2023 */

#include "chess/chessutils.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>


/* Functions */

/**
 * Get piece type associated with Piece object.
 *
 * @param   p   Piece object
 * @return  PieceType type
 **/
Piece       piece_get_type(Piece p) {
    return p & PIECE_TYPE_BM;
}


/**
 * Get color associated with Piece object.
 *
 * @param   p   Piece object
 * @return  PieceType color
 **/
Piece       piece_get_color(Piece p) {
    return p & PIECE_COLOR_BM;
}


/**
 * Create a new Board object.
 *
 * @param   fen     FEN string to build board
 * @return  Pointer to board object (must be freed)
 */
Board *     board_create(const char *fen) {

    // 1. Allocate the board structure.
    Board *b = (Board *) malloc(sizeof(Board));
    if (b) {
        b->data = (Piece *) calloc(BOARD_DIM * BOARD_DIM, sizeof(Piece));        
        b->turn = WHITE;
        b->halfmove_clock = 0;
        b->fullmove_number = 0;
    }

    // 2. Process FEN string.

    //  2(a). Split FEN string into componenets.
    char *new_fen = strdup(fen);
    char board_config[73], turn, castle[5], ep_target[3];
    size_t halfmove_clock = 0, fullmove_number = 0;

    int num_fields = sscanf(new_fen, "%s %c %s %s %lu %lu",
                                board_config, &turn, castle, ep_target, &fullmove_number, &halfmove_clock
                            );
    free(new_fen);
    
    //  2(b). Place pieces on board according to FEN string.
    if (num_fields == 0)
        return b;

    size_t x = 0, y = 0;
    for (char *row = strtok(board_config, "/"); row; row = strtok(NULL, "/")) {
        for (char *c = row; *c; c++) {
            if (isdigit(*c)) {
                size_t num_reps = *c - '0';
                for (size_t i = 0; i < num_reps; i++)
                    x++;
                continue;
            }

            Piece type = strchr(PIECE_TYPE_STR, tolower(*c)) - PIECE_TYPE_STR;
            Piece color = (*c == tolower(*c)) ? BLACK : WHITE;
            *(board_at(b, x++, y)) = type | color;
        }
        x = 0;
        y++;
    }

    //  2(c). Set turn according to FEN string.
    if (num_fields < 2)
        return b;

    b->turn = (turn == 'w') ? WHITE : BLACK;
    
    //  2(d). Set castle availibility according to FEN string.
    if (num_fields < 3)
        return b;
    // TODO

    //  2(e). Set enpassant target according to FEN string.
    if (num_fields < 4)
        return b;
    // TODO

    //  2(f). Set move counters according to FEN string.
    if (num_fields < 6)
        return b;

    b->halfmove_clock = halfmove_clock;
    b->fullmove_number = fullmove_number;

    return b;
}


/**
 * Delete Board object.
 *
 * @param   b   Pointer to Board object to delete.
 **/
void        board_delete(Board *b) {
    free(b->data);
    free(b);
}


/**
 * Get the Piece object at position (x, y) in the board.
 *
 * @param   b   Pointer to Board object.
 * @param   x   X coordinate of piece.
 * @param   y   Y coodrinate of piece.
 * @return  Pointer to Piece at desired location (NULL if out of bounds).
 **/
Piece *     board_at(Board *b, size_t x, size_t y) {
    if (x >= BOARD_DIM || y >= BOARD_DIM)
        return NULL;

    y = BOARD_DIM - y - 1;
    return b->data + x + (y * BOARD_DIM);
}

