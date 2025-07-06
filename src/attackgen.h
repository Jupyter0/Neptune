#ifndef ATTACKGEN_H
#define ATTACKGEN_H

#include "common.h"
#include "board.h"

void AddNonSlidingAttacks(Board board, Piece pieceType, Color color, uint64_t& attacks);
void AddSlidingAttacks(Board board, Piece pieceType, Color color, uint64_t& attacks);
uint64_t CalculateSlidingAttacks(uint64_t sq, Piece pieceType, uint64_t all);
void UpdateAttacks(Board& board);

#endif