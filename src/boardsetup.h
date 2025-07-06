#ifndef BOARDSETUP_H
#define BOARDSETUP_H

#include "common.h"
#include "board.h"
#include "attackgen.h"

void ParsePieces(Board& board, std::string piecesField);
void setBB(Board& board, const std::string& fen);

#endif