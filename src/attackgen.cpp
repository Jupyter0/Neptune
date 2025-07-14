#include "attackgen.h"

using namespace NeptuneInternals;

void GenerateRookAttacks(uint64_t pieces, Board& board, uint64_t& attacks) {
    const uint64_t all = board.generalboards[2];
    while (pieces) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pieces));
        pieces &= pieces -1;

        attacks |= ROOK_ATTACKS(sq, all);
    }
}

void GenerateBishopAttacks(uint64_t pieces, Board& board, uint64_t& attacks) {
    const uint64_t all = board.generalboards[2];
    while (pieces) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pieces));
        pieces &= pieces -1;

        attacks |= BISHOP_ATTACKS(sq, all);
    }
}

void GenerateKnightAttacks(uint64_t pieces, Color color, uint64_t& attacks) {
    while (pieces) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pieces));
        pieces &= pieces -1;

        attacks |= attacksBB[KNIGHT-1][color][sq];
    }
}

void GenerateKingAttacks(uint64_t pieces, Color color, uint64_t& attacks) {
    while (pieces) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pieces));
        pieces &= pieces -1;

        attacks |= attacksBB[KING-1][color][sq];
    }
}

void GeneratePawnAttacks(uint64_t pieces, Color color, uint64_t& attacks) {
    while (pieces) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pieces));
        pieces &= pieces -1;

        attacks |= attacksBB[PAWN-1][color][sq];
    }
}

void AddNonSlidingAttacks(Board& board, Color color, uint64_t& attacks) {
    GenerateKnightAttacks(board.bitboards[color][KNIGHT-1], color, attacks);
    GenerateKingAttacks(board.bitboards[color][KING-1], color, attacks);
    GeneratePawnAttacks(board.bitboards[color][PAWN-1], color, attacks);
}

void AddSlidingAttacks(Board& board, Color color, uint64_t& attacks) {
    GenerateRookAttacks(board.bitboards[color][ROOK-1], board, attacks);
    GenerateBishopAttacks(board.bitboards[color][BISHOP-1], board, attacks);

    GenerateRookAttacks(board.bitboards[color][QUEEN-1], board, attacks);
    GenerateBishopAttacks(board.bitboards[color][QUEEN-1], board, attacks);
}

void UpdateAttacks(Board& board) {
    board.whiteAttacks = 0ULL;
    board.blackAttacks = 0ULL;

    AddSlidingAttacks(board, WHITE, board.whiteAttacks);
    AddSlidingAttacks(board, BLACK, board.blackAttacks);

    AddNonSlidingAttacks(board, WHITE, board.whiteAttacks);
    AddNonSlidingAttacks(board, BLACK, board.blackAttacks);
}