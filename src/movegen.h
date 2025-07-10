#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "common.h"
#include "board.h"
#include "magicbitboards.h"

int GenerateLegalMoves(Board& board, Move* moves);
int GeneratePseudoLegalMoves(Board& board, Move* moves);

#endif