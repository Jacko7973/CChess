/* chess_test.c */
/* Jack O'Connor 2023 */

#include <stdio.h>

#include "chess/chessutils.h"
#include "chess/terminalchess.h"
#include "chess/bitboard.h"


const char *DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

int main(int argc, char *argv[]) {
    
    Board *b = board_create(DEFAULT_FEN);

    b->data[3] = PAWN | WHITE;
    b->data[11] = PAWN | BLACK;

    format_board(b, stdout);

    board_delete(b);

    return 0;
}
