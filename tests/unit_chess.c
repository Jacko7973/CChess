/* libchess
 * Jack O'Connor 2025
 * tests/unit_chess.c
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "chessboard.h"


/* Constants */

// https://en.wikipedia.org/wiki/Shannon_number
const size_t SHANNON[][2] = {
    {1,     20},
    {2,     400},
    {3,     8902},
    {4,     197281},
    {5,     4865609},
    // {6,     119060324},
    // {7,     3195901860},
    // {8,     84998978956},
    // {9,     2439530234167},
    // {10,   69352859712417},
    // {11,   2097651003696806},
    // {12,   62854969236701747},
    // {13,   1981066775000396239},
    // {14,   61885021521585529237},
    // {15,   2015099950053364471960}
};

const char * KIWIPETE_FEN = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
const size_t KIWIPETE[][2] = {
    {1,		48},
    {2,		2039},
    {3,		97862},
    {4,		4085603},
    {5,		193690690},
    {6,		8031647685}
};


/* Unit Tests */

bool    test_01_perft_results() {
    
    fprintf(stdout, "Testing perft results...\n");

    bool success = true;
    fprintf(stdout, "Default initial positions...\n");
    for (size_t i = 0; i < sizeof(SHANNON) / sizeof(SHANNON[0]); i++) {
        size_t ply = SHANNON[i][0];
        size_t target = SHANNON[i][1];

        ChessBoard *cb = chessboard_create(NULL);
        // size_t position_count = chessboard_count_moves(cb, WHITE, ply);
        // size_t difference = (target > position_count) ? 
        //     target - position_count : position_count - target;
        // if (difference) success = false;

        // fprintf(stdout, "[%c] (Ply=%2lu) Target=%10lu | Actual=%10lu",
        //         (difference) ? 'X' : '.', ply, target, position_count);
        // if (difference) fprintf(stdout, " (off by %lu)", difference);
        // fprintf(stdout, "\n");

        // success = success && !difference;
        // if (!success) break;
    }

    fprintf(stdout, "\nKiwipete initial position...(%s)\n", KIWIPETE_FEN);
    for (size_t i = 0; i < sizeof(KIWIPETE) / sizeof(KIWIPETE[0]); i++) {
        size_t ply = KIWIPETE[i][0];
        size_t target = KIWIPETE[i][1];

        ChessBoard *cb = chessboard_create(KIWIPETE_FEN);
        // size_t position_count = chessboard_count_moves(cb, WHITE, ply);
        // size_t difference = (target > position_count) ? 
        //     target - position_count : position_count - target;
        // if (difference) success = false;

        // fprintf(stdout, "[%c] (Ply=%2lu) Target=%10lu | Actual=%10lu",
        //         (difference) ? 'X' : '.', ply, target, position_count);
        // if (difference) fprintf(stdout, " (off by %lu)", difference);
        // fprintf(stdout, "\n");

        // success = success && !difference;
        // if (!success) break;
        
    }


    return success;
}



/* Main Execution */

int main(int argc, char *argv[]) {

    int failures = 0;

    failures += test_01_perft_results() ? 0 : 1;

    return EXIT_SUCCESS;
}

