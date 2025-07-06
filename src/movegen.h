#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "common.h"
#include "board.h"

void GenerateCastlingMoves(uint8_t castlingRights, uint64_t all, std::vector<Move>& moves, Color color, uint64_t whiteAttacks, uint64_t blackAttacks);
void GenerateSlidingMoves(uint64_t pieces, const int directions[][2], int dirCount, uint64_t own, uint64_t enemy, std::vector<Move>& moves);
void GenerateNonSlidingMoves(uint64_t pieces, const uint64_t attackTable[64], uint64_t own, std::vector<Move>& moves);
void GeneratePawnMoves(uint64_t pawns, uint64_t enemy, uint64_t all, std::vector<Move>& moves, Color color, uint8_t epSquare);
std::vector<Move> GeneratePseudoLegalMoves(Board& board, bool isWhiteToMove);
std::vector<Move> GenerateLegalMoves(Board& board);

#endif