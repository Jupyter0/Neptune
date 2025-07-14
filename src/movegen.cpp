#include "movegen.h"
#include "board.h"

using namespace NeptuneInternals;

void GenerateCastlingMoves(uint8_t castlingRights, uint64_t all, Move* moves, Color color, uint64_t whiteAttacks, uint64_t blackAttacks, int& count) {
    if (color == WHITE) {
        if (((castlingRights & 0b1000) != 0) && ((all & castlingBB[0]) == 0) && ((blackAttacks & bitMasks[f1]) == 0)) {
            moves[count++] = Move(e1, g1);
        }
        if (((castlingRights & 0b0100) != 0) && ((all & castlingBB[1]) == 0) && ((blackAttacks & bitMasks[d1]) == 0) && ((blackAttacks & bitMasks[c1]) == 0)) {
            moves[count++] = Move(e1, c1);
        }
    } else {
        if (((castlingRights & 0b0010) != 0) && ((all & castlingBB[2]) == 0) && ((whiteAttacks & bitMasks[f8]) == 0)) {
            moves[count++] = Move(e8, g8);
        }
        if (((castlingRights & 0b0001) != 0) && ((all & castlingBB[3]) == 0) && ((whiteAttacks & bitMasks[d8]) == 0) && ((whiteAttacks & bitMasks[c8]) == 0)) {
            moves[count++] = Move(e8, c8);
        }
    }
}

void GenerateRookMoves(uint64_t pieces, Board& board, Color color, Move* moves, int& count) {
    const uint64_t all = board.generalboards[2];
    const uint64_t friendly = board.generalboards[color];
    while (pieces) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pieces));
        pieces &= pieces -1;

        uint64_t attacks = ROOK_ATTACKS(sq, all) & ~friendly;

        while (attacks) {
            uint8_t to = static_cast<uint8_t>(__builtin_ctzll(attacks));
            attacks &= attacks - 1;
            moves[count++] = Move(sq, to);
        }
    }
}

void GenerateBishopMoves(uint64_t pieces, Board& board, Color color, Move* moves, int& count) {
    const uint64_t all = board.generalboards[2];
    const uint64_t friendly = board.generalboards[color];
    while (pieces) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pieces));
        pieces &= pieces -1;

        uint64_t attacks = BISHOP_ATTACKS(sq, all) & ~friendly;

        while (attacks) {
            uint8_t to = static_cast<uint8_t>(__builtin_ctzll(attacks));
            attacks &= attacks - 1;
            moves[count++] = Move(sq, to);
        }
    }
}

void GenerateSlidingMoves(Board& board, Color color, Move* moves, int& count) {
    GenerateRookMoves(board.bitboards[color][3], board, color, moves, count);
    GenerateBishopMoves(board.bitboards[color][2], board, color, moves, count);

    GenerateRookMoves(board.bitboards[color][4], board, color, moves, count);
    GenerateBishopMoves(board.bitboards[color][4], board, color, moves, count);
}

void GenerateKnightMoves(uint64_t pieces, Board& board, Color color, Move* moves, int& count) {
    const uint64_t friendly = board.generalboards[color];
    while(pieces) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pieces));
        pieces &= pieces - 1;

        uint64_t attacks = attacksBB[KNIGHT-1][color][sq] & ~friendly;

        while (attacks) {
            uint8_t to = static_cast<uint8_t>(__builtin_ctzll(attacks));
            attacks &= attacks - 1;
            moves[count++] = Move(sq, to);
        }
    }
}

void GenerateKingMoves(uint64_t pieces, Board& board, Color color, Move* moves, int& count) {
    const uint64_t friendly = board.generalboards[color];
    while(pieces) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pieces));
        pieces &= pieces - 1;

        uint64_t attacks = attacksBB[KING-1][color][sq] & ~friendly;

        while (attacks) {
            uint8_t to = static_cast<uint8_t>(__builtin_ctzll(attacks));
            attacks &= attacks - 1;
            moves[count++] = Move(sq, to);
        }
    }
}

void GenerateNonSlidingMoves(Board& board, Color color, Move* moves, int& count) {
    GenerateKnightMoves(board.bitboards[color][1], board, color, moves, count);
    GenerateKingMoves(board.bitboards[color][5], board, color, moves, count);
}

void GeneratePawnMoves(uint64_t pawns, uint64_t enemy, uint64_t all, Move* moves, Color color, int& count, uint8_t epSquare = 0) {
    int forward = (color == WHITE) ? 8 : -8;
    uint8_t startRank = (color == WHITE) ? 1 : 6;
    uint8_t promotionRank = (color == WHITE) ? 6 : 1;

    while (pawns) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pawns));
        pawns &= pawns - 1;

        uint8_t rank = sq >> 3;

        int step = sq + forward;
        if (step < 0 || step >= 64) continue;
        uint8_t oneStep = static_cast<uint8_t>(step);
        if (oneStep < 64 && !(all & bitMasks[oneStep])) {
            if (rank == promotionRank) {
                moves[count++] = Move(sq, oneStep, 'q');
                moves[count++] = Move(sq, oneStep, 'r');
                moves[count++] = Move(sq, oneStep, 'b');
                moves[count++] = Move(sq, oneStep, 'n');
            } else {
                moves[count++] = Move(sq, oneStep);

                if (rank == startRank) {
                    uint8_t twoStep = static_cast<uint8_t>(sq + 2 * forward);
                    if (!(all & bitMasks[twoStep]) && !(all & bitMasks[oneStep])) {
                        moves[count++] = Move(sq, twoStep);
                    }
                }
            }
        }

        uint64_t attacks = attacksBB[WHITE][color][sq];
        uint64_t normalCaptures = attacks & enemy;

        while (normalCaptures) {
            uint8_t targetSq = static_cast<uint8_t>(__builtin_ctzll(normalCaptures));
            normalCaptures &= normalCaptures - 1;

            if (rank == promotionRank) {
                moves[count++] = Move(sq, targetSq, 'q');
                moves[count++] = Move(sq, targetSq, 'r');
                moves[count++] = Move(sq, targetSq, 'b');
                moves[count++] = Move(sq, targetSq, 'n');
            } else {
                moves[count++] = Move(sq, targetSq);
            }
        }

        if (epSquare != 0) {
            uint64_t epBB = bitMasks[epSquare];
            if ((attacks & epBB) != 0) {
                moves[count++] = Move(static_cast<uint8_t>(sq), epSquare, 0, true);
            }
        }
    }
}

int GeneratePseudoLegalMoves(Board& board, Move* moves /*Should be empty!*/) {
    int count = 0;
    uint8_t epTarget = board.hasEnPassant() ? board.getEnPassantTarget() : 0;
    Color color = static_cast<Color>(board.whiteToMove ? WHITE : BLACK);

    GenerateSlidingMoves(board, color, moves, count);
    GenerateNonSlidingMoves(board, color, moves, count);
    GeneratePawnMoves(board.bitboards[color][PAWN-1], board.generalboards[1-color], board.generalboards[2], moves, color, count, epTarget);

    if (!board.isKingInCheck(board.whiteToMove)) 
        GenerateCastlingMoves(board.castlingRights, board.generalboards[2], moves, color, board.whiteAttacks, board.blackAttacks, count);

    return count;
};

int GenerateLegalMoves(Board& board, Move* moves /*Should be empty!*/) {
    int count = 0;
    Move pseudoMoves[256];
    int pseudoMoveCount = GeneratePseudoLegalMoves(board, pseudoMoves);

    for (int i = 0; i < pseudoMoveCount; ++i) {
        Move& move = pseudoMoves[i];
        Color us = board.whiteToMove ? WHITE : BLACK;
        board.make_move(move);
        if (!board.isSquareAttacked(board.kings[us], static_cast<Color>(!us))) { // check if own king is not in check
            moves[count++] = move;
        }
        board.unmake_move();
    }

    return count;
}