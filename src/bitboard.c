/* bitboard.c */
/* Jack O'Connor 2023 */

#include "chess/bitboard.h"


/* Functions */

/**
 * Set a bit on a Bitboard object.
 *
 * @param bb    Pointer to Bitboard object.
 * @param x     X coordinate of bit.
 * @param y     Y coordinate of bit.
 **/
void        bitboard_set(Bitboard *bb, size_t x, size_t y) {
    if (x >= BOARD_DIM || y >= BOARD_DIM)
        return;
    
    *bb = *bb | ((uint64_t)1 << (x + y * BOARD_DIM));
}


/**
 * Unset a bit on a Bitboard object.
 *
 * @param bb    Pointer to Bitboard object.
 * @param x     X coordinate of bit.
 * @param y     Y coordinate of bit.
 **/
void        bitboard_unset(Bitboard *bb, size_t x, size_t y) {
    if (x >= BOARD_DIM || y >= BOARD_DIM)
        return;
    
    *bb = *bb & ~((uint64_t)1 << (x + y * BOARD_DIM));
}


/**
 * Toggle a bit on a Bitboard object.
 *
 * @param bb    Pointer to Bitboard object.
 * @param x     X coordinate of bit.
 * @param y     Y coordinate of bit.
 **/
void        bitboard_toggle(Bitboard *bb, size_t x, size_t y) {
    if (x >= BOARD_DIM || y >= BOARD_DIM)
        return;
    
    *bb = *bb ^ ((uint64_t)1 << (x + y * BOARD_DIM));
}


/**
 * Get the status of a bit on a Bitboard object.
 *
 * @param bb    Pointer to Bitboard object.
 * @param x     X coordinate of bit.
 * @param y     Y coordinate of bit.
 * @return      Status of the bit.
 **/
bool        bitboard_search(Bitboard *bb, size_t x, size_t y) {
    return (*bb >> (x + y * BOARD_DIM)) & (uint64_t)1;
}


/**
 * Format a bitboard to the specified file stream.
 * 
 * @param bb        Pointer to Bitboard object.
 * @param stream    File stream to format to.
*/
void        bitboard_format(Bitboard *bb, FILE* stream) {
    Bitboard tmp = *bb;

    size_t i = 0;
    while (i < BOARD_DIM * BOARD_DIM) {
        if (i != 0 && i % BOARD_DIM == 0)
            fprintf(stream, "\n");
        fprintf(stream, "%c", ((tmp & (uint64_t)1) == 1) ? '#' : '.');

        tmp = tmp >> 1;
        i++;
    }
    fprintf(stream, "\n");
}