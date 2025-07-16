#include "search.h"

int Quiescence(Board& board, int alpha, int beta, bool maximizingPlayer) {
    int stand_pat = Evaluate(board);

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

int MiniMax(Board& board, int depth, bool maximizingPlayer, PVLine& line, TranspositionTable& tt, int alpha, int beta) {
    TTEntry* entry = tt.probe(board.zobristKey);
    Move ttMove;

    if (entry && entry->zobristKey == board.zobristKey) {
        if (entry->depth >= depth) {
            switch (entry->flag) {
                case EXACT: return entry->eval;
                case LOWERBOUND: alpha = std::max(alpha, entry->eval); break;
                case UPPERBOUND: beta = std::min(beta, entry->eval); break;
            }

            if (alpha >= beta) {
                return entry->eval;
            }
        }
        ttMove = entry->move;
    }

    if (depth == 0) { line.clear(); return Quiescence(board, alpha, beta, maximizingPlayer); }

    Move moves[256];
    int pseudoMoveCount = GeneratePseudoLegalMoves(board, moves);

    if (ttMove.from != ttMove.to) {
        for (int i = 0; i < pseudoMoveCount; ++i) {
            if (moves[i] == ttMove) {
                std::swap(moves[0], moves[i]);
                break;
            }
        }
    }

    Color toPlay = maximizingPlayer ? WHITE : BLACK;
    int bestScore = maximizingPlayer ? -infinity + board.ply : infinity - board.ply;
    PVLine bestLine;

    bool foundLegal = false;
    int alphaOrig = alpha;
    for (int i = 0; i < pseudoMoveCount; ++i) {
        Move move = moves[i];
        board.MakeMove(move);

        if (board.isSquareAttacked(board.kings[toPlay], static_cast<Color>(1-toPlay))) { board.UnmakeMove(); continue; }
        foundLegal = true;
        PVLine currentLine;
        int score = MiniMax(board, depth - 1, !maximizingPlayer, currentLine, tt, alpha, beta);
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

    EvalFlag flag;
    if (bestScore <= alphaOrig) {
        flag = UPPERBOUND;
    } else if (bestScore >= beta) {
        flag = LOWERBOUND;
    } else {
        flag = EXACT;
    }

    if (foundLegal && !bestLine.empty()) {
        tt.store(board.zobristKey, TTEntry{
            board.zobristKey,
            depth,
            bestScore,
            flag,
            bestLine[0]
        });
    }

    return bestScore;
}