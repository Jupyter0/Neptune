#ifndef PERFT_H
#define PERFT_H

#include "common.h"
#include "board.h"
#include "movegen.h"

uint64_t perft(Board& board, int depth);
uint64_t perft_divide(Board& board, int depth);
void perft_debug(Board& board, int depth);

#endif