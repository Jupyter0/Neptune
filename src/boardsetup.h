#ifndef BOARDSETUP_H
#define BOARDSETUP_H

#include "board.h"

void ParsePieces(Board& board, std::string piecesField);
void setBB(Board& board, const std::string& fen);

#endif