/* bitboard.h */
/* Jack O'Connor 2023 */

#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "chessutils.h"

/* Macros */

#ifndef BOARD_DIM
#define BOARD_DIM (8)
#endif

/* Custom Types */

typedef uint64_t Bitboard;

/* Function Headers */

void        bitboard_set(Bitboard *bb, size_t x, size_t y);
void        bitboard_unset(Bitboard *bb, size_t x, size_t y);
void        bitboard_toggle(Bitboard *bb, size_t x, size_t y);

bool        bitboard_search(Bitboard *bb, size_t x, size_t y);

void        bitboard_format(Bitboard *bb, FILE* stream);

