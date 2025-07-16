#include "eval.h"
#include "diagnostics.h"

int Evaluate(Board& board) {
    int score = 0;

    score += pieceValue[PAWN] * (__builtin_popcountll(board.bitboards[WHITE][PAWN-1]));
    score += pieceValue[KNIGHT] * (__builtin_popcountll(board.bitboards[WHITE][KNIGHT-1]));
    score += pieceValue[BISHOP] * (__builtin_popcountll(board.bitboards[WHITE][BISHOP-1]));
    score += pieceValue[ROOK] * (__builtin_popcountll(board.bitboards[WHITE][ROOK-1]));
    score += pieceValue[QUEEN] * (__builtin_popcountll(board.bitboards[WHITE][QUEEN-1]));

    score -= pieceValue[PAWN] * (__builtin_popcountll(board.bitboards[BLACK][PAWN-1]));
    score -= pieceValue[KNIGHT] * (__builtin_popcountll(board.bitboards[BLACK][KNIGHT-1]));
    score -= pieceValue[BISHOP] * (__builtin_popcountll(board.bitboards[BLACK][BISHOP-1]));
    score -= pieceValue[ROOK] * (__builtin_popcountll(board.bitboards[BLACK][ROOK-1]));
    score -= pieceValue[QUEEN] * (__builtin_popcountll(board.bitboards[BLACK][QUEEN-1]));

    ++nodesSearched;
    return score;
}