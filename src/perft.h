#ifndef PERFT_H
#define PERFT_H

#include "common.h"
#include "board.h"
#include "movegen.h"

uint64_t Perft(Board& board, int depth);
uint64_t PerftDivide(Board& board, int depth);
void PerftDebug(Board& board, int depth);

#endif