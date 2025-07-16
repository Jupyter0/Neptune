#include "eval.h"

int Evaluate(Board& board) {
    int score = 0;

    for (int square = 0; square < 64; ++square) {
        Piece piece = board.pieceAt[square];
        if (piece == EMPTY) continue;

        Color color = (board.generalboards[WHITE] & bitMasks[square]) ? WHITE : BLACK;
        int flippedSq = (color == WHITE) ? square : (56 ^ square);
        int pstValue = PieceValue(piece, flippedSq, __builtin_popcountll(board.generalboards[2]));

        score += (color == WHITE ? pstValue : -pstValue);
    }

    ++nodesSearched;
    return score;
}