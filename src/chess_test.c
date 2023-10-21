/* chess_test.c */
/* Jack O'Connor 2023 */

#include <stdio.h>

#include "chessutils.h"
#include "terminalchess.h"


int main(int argc, char *argv[]) {
    
    Board *b = board_create("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

    b->data[3] = PAWN | WHITE;
    b->data[11] = PAWN | BLACK;

    format_board(b, stdout);

    board_delete(b);

    return 0;
}
