#ifndef SEARCH_H
#define SEARCH_H

#include "common.h"
#include "board.h"
#include "movegen.h"
#include "eval.h"

using PVLine = std::vector<Move>;

int MiniMax(Board& board, int depth, bool maximizingPlayer, PVLine& line, int alpha = -infinity, int beta = infinity);
int Quiescence(Board& board, int alpha, int beta, bool maximizingPlayer);

#endif