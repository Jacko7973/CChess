/* libchess
 * Jack O'Connor 2025
 * include/bitboard.h
 */

#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* Types */

typedef uint64_t Bitboard;


/* Macro Functions */

#define bitboard_set(b, file, rank, value)      (b = ((value) ? b | (1L << (rank * 8 + file)) : b & ~(1L << (rank * 8 + file))))
#define bitboard_get(b, file, rank)             ((b >> (rank * 8 + file) & 1L))

/* Function Headers */

void    bitboard_dump(Bitboard *b, FILE *stream);


#endif

