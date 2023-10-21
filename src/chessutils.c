/* chessutils.c */
/* Jack O'Connor 2023 */

#include "chessutils.h"

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
Board *     board_create(char *fen) {

    // 1). Allocate the board structure.
    Board *b = (Board *) malloc(sizeof(Board));
    if (b) {
        b->data = (Piece *) calloc(BOARD_DIM * BOARD_DIM, sizeof(Piece));        
        b->turn = WHITE;
        b->halfmove_clock = 0;
        b->fullmove_number = 0;
    }

    // 2). Process FEN string.
    //  TODO: handle remaining components of FEN string.

    char *new_fen = strdup(fen);

    size_t x = 0, y = 0;
    for (char *row = strtok(new_fen, "/"); row; row = strtok(NULL, "/")) {
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

    free(new_fen);

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

