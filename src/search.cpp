#include "search.h"
#include "diagnostics.h"

int Quiescence(Board& board, int alpha, int beta, bool maximizingPlayer) {
    int stand_pat = Evaluate(board);

    ++nodesSearched;

    if (maximizingPlayer) {
        if (stand_pat >= beta) return beta;
        if (alpha < stand_pat) alpha = stand_pat;
    } else {
        if (stand_pat <= alpha) return alpha;
        if (beta > stand_pat) beta = stand_pat;
    }

    Move captures[256];
    int captureCount = GenerateCaptures(board, captures);

    for (int i = 0; i < captureCount; ++i) {
        board.MakeMove(captures[i]);

        if (board.isSquareAttacked(board.kings[maximizingPlayer ? WHITE : BLACK], maximizingPlayer ? BLACK : WHITE)) {
            board.UnmakeMove();
            continue;
        }

        int score = Quiescence(board, alpha, beta, !maximizingPlayer);

        board.UnmakeMove();

        if (maximizingPlayer) {
            if (score >= beta) return beta;
            if (score > alpha) alpha = score;
        } else {
            if (score <= alpha) return alpha;
            if (score < beta) beta = score;
        }
    }

    return maximizingPlayer ? alpha : beta;
}

int MiniMax(Board& board, int depth, bool maximizingPlayer, PVLine& line, int alpha, int beta) {
    if (depth == 0) { line.clear(); return Quiescence(board, alpha, beta, maximizingPlayer); }

    Move moves[256];
    int pseudoMoveCount = GeneratePseudoLegalMoves(board, moves);

    Color toPlay = maximizingPlayer ? WHITE : BLACK;
    int bestScore = maximizingPlayer ? -infinity + board.ply : infinity - board.ply;
    PVLine bestLine;

    bool foundLegal = false;
    for (int i = 0; i < pseudoMoveCount; ++i) {
        Move move = moves[i];
        board.MakeMove(move);

        if (board.isSquareAttacked(board.kings[toPlay], static_cast<Color>(1-toPlay))) { board.UnmakeMove(); continue; }
        foundLegal = true;
        PVLine currentLine;
        int score = MiniMax(board, depth - 1, !maximizingPlayer, currentLine, alpha, beta);
        if (maximizingPlayer) {
            if (score > bestScore) {
                bestScore = score;
                bestLine = currentLine;
                bestLine.insert(bestLine.begin(), move);
            }
            alpha = std::max(alpha, score);
        } else {
            if (score < bestScore) {
                bestScore = score;
                bestLine = currentLine;
                bestLine.insert(bestLine.begin(), move);
            }
            beta = std::min(beta, score);
        }
        board.UnmakeMove();
        if (beta <= alpha) break;
    }

    if (!foundLegal) {
        if (!(board.isSquareAttacked(board.kings[WHITE], BLACK) || board.isSquareAttacked(board.kings[BLACK], WHITE))) {
            return 0;
        }
    }
    line = bestLine;
    return bestScore;
}