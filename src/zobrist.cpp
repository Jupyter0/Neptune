#include "zobrist.h"

uint64_t rand64() {
    static std::mt19937_64 rng(934287590234);
    return rng();
}

void InitZobrist() {
    for (int color = 0; color < 2; ++color) {
        for (int piece = 0; piece < 6; ++piece) {
            for (int square = 0; square < 64; ++square) {
                zobristPiece[color][piece][square] = rand64();
            }
        }
    }

    for (int file = 0; file < 8; ++file) {
        zobristEnPassant[file] = rand64();
    }

    for (int i = 0; i < 16; ++i) {
        zobristCastling[i] = rand64();
    }

    zobristSideToMove = rand64();
}