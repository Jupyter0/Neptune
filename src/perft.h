#ifndef PERFT_H
#define PERFT_H

#include "common.h"
#include "board.h"
#include "movegen.h"

int perft(Board& board, int depth);
int perft_divide(Board& board, int depth);
void perft_debug(Board& board, int depth);

#endif