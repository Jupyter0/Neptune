#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "common.h"
#include "board.h"
#include "magicbitboards.h"

using namespace NeptuneInternals;

int GenerateLegalMoves(Board& board, Move* moves);
int GenerateCaptures(Board& board, Move* moves);

#endif