#ifndef BOARDSETUP_H
#define BOARDSETUP_H

#include "common.h"
#include "board.h"

void ParsePieces(Board& board, std::string piecesField);
void SetBB(Board& board, const std::string& fen);
void Position(Board& board, const std::string& input);

#endif