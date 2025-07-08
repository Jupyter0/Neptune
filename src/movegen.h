#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "common.h"
#include "board.h"
#include "magicbitboards.h"

std::vector<Move> GenerateLegalMoves(Board& board);

#endif