/* terminalchess.c */
/* Jack O'Connor 2023 */

#include "terminalchess.h"

#include <ctype.h>

/* Constants */

const char *PIECE_TYPE_STR = " prnbqk";

/* Functions */

/**
 * Display the Board object on the specified file stream
 *
 * @param   b       Pointer to Board object
 * @param   stream  File stream to output to
 **/
void    format_board(Board *b, FILE *stream) {

    for (size_t i = 0; i < BOARD_DIM; i++) {
        for (size_t j = 0; j < BOARD_DIM; j++)
            fprintf(stream, "+-----");
        fprintf(stream, "+\n");
        for (size_t j = 0; j < BOARD_DIM; j++)
            fprintf(stream, "|     ");
        fprintf(stream, "|\n");

        for (size_t j = 0; j < BOARD_DIM; j++) {
            Piece *curr = board_at(b, j, i);

            Piece color = piece_get_color(*curr);
            Piece type = piece_get_type(*curr);
            char c = PIECE_TYPE_STR[type];
            if (color == WHITE) c = toupper(c);

            fprintf(stream, "|  %c  ", c);
        }
        fprintf(stream, "|\n");

        for (size_t j = 0; j < BOARD_DIM; j++)
            fprintf(stream, "|     ");
        fprintf(stream, "|\n");
    
    }

    for (size_t i = 0; i < BOARD_DIM; i++)
        fprintf(stream, "+-----");
    fprintf(stream, "+\n");
}

