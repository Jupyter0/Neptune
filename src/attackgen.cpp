#include "attackgen.h"

void AddNonSlidingAttacks(Board board, Piece pieceType, Color color, uint64_t& attacks) {
    if (pieceType == BISHOP || pieceType == ROOK || pieceType == QUEEN || pieceType == EMPTY) return;
    uint64_t p = board.bitboards[color][pieceType-1];
    while(p) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(p));
        p &= p - 1;

        attacks |= attacksBB[pieceType-1][color][sq];
    }
}

void AddSlidingAttacks(Board board, Piece pieceType, Color color, uint64_t& attacks) {
    if (pieceType == KNIGHT || pieceType == KING || pieceType == PAWN || pieceType == EMPTY) return;
    uint64_t p = board.bitboards[color][pieceType-1];
    while(p) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(p));
        p &= p - 1;

        attacks |= CalculateSlidingAttacks(sq, static_cast<Piece>(pieceType), board.generalboards[2]);
    }
}

uint64_t CalculateSlidingAttacks(uint64_t sq, Piece pieceType, uint64_t all) {
    uint64_t rank = sq >> 3;
    uint64_t file = sq & 7;
    uint64_t attacks = 0;

    for (const int (&d)[2] : dirs[pieceType-1]) {
        if (d[0] == 0 && d[1] == 0) break;
        int r = static_cast<int>(rank) + d[0];
        int f = static_cast<int>(file) + d[1];
        while (r >= 0 && r < 8 && f >= 0 && f < 8) {
            int target = r * 8 + f;
            uint64_t targetBB = bitMasks[target];
            attacks |= targetBB;
            if (all & targetBB) break;
            r += d[0];
            f += d[1];
        }           
    }
    return attacks;
}

void UpdateAttacks(Board& board) {
    board.whiteAttacks = 0ULL;
    board.blackAttacks = 0ULL;

    AddNonSlidingAttacks(board, KNIGHT, WHITE, board.whiteAttacks);
    AddNonSlidingAttacks(board, KING, WHITE, board.whiteAttacks);
    AddNonSlidingAttacks(board, PAWN, WHITE, board.whiteAttacks);
    AddNonSlidingAttacks(board, KNIGHT, BLACK, board.blackAttacks);
    AddNonSlidingAttacks(board, KING, BLACK, board.blackAttacks);
    AddNonSlidingAttacks(board, PAWN, BLACK, board.blackAttacks);

    AddSlidingAttacks(board, BISHOP, WHITE, board.whiteAttacks);
    AddSlidingAttacks(board, ROOK, WHITE, board.whiteAttacks);
    AddSlidingAttacks(board, QUEEN, WHITE, board.whiteAttacks);
    AddSlidingAttacks(board, BISHOP, BLACK, board.blackAttacks);
    AddSlidingAttacks(board, ROOK, BLACK, board.blackAttacks);
    AddSlidingAttacks(board, QUEEN, BLACK, board.blackAttacks);
}