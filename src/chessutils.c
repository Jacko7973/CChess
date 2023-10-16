/* chessutils.c */
/* Jack O'Connor 2023 */

#include "chessutils.h"


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
    // TODO: Process FEN string.

    Board *b = (Board *) malloc(sizeof(Board));
    if (b) {
        b->data = (Piece *) calloc(BOARD_DIM * BOARD_DIM, sizeof(Piece));        
        b->turn = WHITE;
        b->halfmove_clock = 0;
        b->fullmove_number = 0;
    }
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

