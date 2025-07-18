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
        Move capture = captures[i];

        board.MakeMove(capture);

        if (board.isSquareAttacked(board.kings[maximizingPlayer ? WHITE : BLACK], maximizingPlayer ? BLACK : WHITE, false)) {
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
    int moveCount = GenerateLegalMoves(board, moves);

    if (ttMove.from != ttMove.to) {
        for (int i = 0; i < moveCount; ++i) {
            if (moves[i] == ttMove) {
                std::swap(moves[1], moves[i]);
                break;
            }
        }
    }

    int bestScore = maximizingPlayer ? -infinity + board.ply : infinity - board.ply;
    PVLine bestLine;

    int alphaOrig = alpha;
    for (int i = 0; i < moveCount; ++i) {
        Move move = moves[i];
        board.MakeMove(move);
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

    if (moveCount == 0) {
        if (!(board.isSquareAttacked(board.kings[WHITE], BLACK, false) || board.isSquareAttacked(board.kings[BLACK], WHITE, false))) {
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

    if (moveCount != 0 && !bestLine.empty()) {
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

int IterativeSearch(Board& board, int maxDepth, PVLine& line) {
    TranspositionTable tt(25);
    Move bestMove;
    int finalScore = 0;
    for (int currentDepth = 1; currentDepth <= maxDepth; ++currentDepth) {
        PVLine currentLine;
        int score = MiniMax(board, currentDepth, board.whiteToMove, currentLine, tt);
        if (!currentLine.empty()) {
            bestMove = currentLine[0];
            line = currentLine;
            finalScore = score;
        }
        
        std::cout << "Searched depth " << currentDepth << ", eval: " << score << "\nCurrent Best: "
        << MoveToUCI(bestMove) << "\n";

        tt.store(board.zobristKey, TTEntry{
            board.zobristKey,
            currentDepth,
            score,
            EXACT,
            bestMove
        });
    }
    return finalScore;
}