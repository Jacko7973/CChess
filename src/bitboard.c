/* libchess
 * Jack O'Connor 2025
 * src/bitboard.c
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "bitboard.h"


/* External Functions */

void    bitboard_set(Bitboard *b, uint8_t file, uint8_t rank, bool value) {
    if (file >= 8 || rank >= 8) return;

    if (value)
        *b |= 1L << ((rank * 8) + file);
    else
        *b &= ~(1L << ((rank * 8) + file));
}

bool    bitboard_get(Bitboard *b, uint8_t file, uint8_t rank) {
    if (file >= 8 || rank >= 8) return false;
    return (*b >> ((rank * 8) + file)) & 1L;
    
}

void    bitboard_dump(Bitboard *b, FILE *stream) {
    for (int8_t rank = 7; rank >= 0; rank--) {
        for (int8_t file = 0; file < 8; file++) {
            fprintf(stream, "%lu", (*b >> ((rank * 8) + file) & 1L));
        }
        fprintf(stream, "\n");
    }
}

