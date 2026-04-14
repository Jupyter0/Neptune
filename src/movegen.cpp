#include "movegen.h"
#include "board.h"

using namespace NeptuneInternals;

void GenerateCastlingMoves(Board& board, uint64_t all, Move* moves, Color side, int& count) {
    if (board.isKingInCheck(side == WHITE)) return;

    uint8_t from = side == WHITE ? e1 : e8;
    
    if (side == WHITE) {
        if (board.castlingRights & 0b1000) {
            uint64_t clearance = bitMasks[f1] | bitMasks[g1];
            uint8_t travel[2];
            travel[0] = f1;
            travel[1] = g1;
            if (!(all & clearance) && !board.isSquareAttacked(travel, 2, BLACK, false)) {
                moves[count++] = Move(from, g1);
            }
        } if (board.castlingRights & 0b0100) {
            uint64_t clearance = bitMasks[d1] | bitMasks[c1] | bitMasks[b1];
            uint8_t travel[2];
            travel[0] = d1;
            travel[1] = c1;
            if (!(all & clearance) && !board.isSquareAttacked(travel, 2, BLACK, false)) {
                moves[count++] = Move(from, c1);
            }
        }
    } else {
        if (board.castlingRights & 0b0010) {
            uint64_t clearance = bitMasks[f8] | bitMasks[g8];
            uint8_t travel[2];
            travel[0] = f8;
            travel[1] = g8;
            if (!(all & clearance) && !board.isSquareAttacked(travel, 2, WHITE, false)) {
                moves[count++] = Move(from, g8);
            }
        } if (board.castlingRights & 0b0001) {
            uint64_t clearance = bitMasks[d8] | bitMasks[c8] | bitMasks[b8];
            uint8_t travel[2];
            travel[0] = d8;
            travel[1] = c8;
            if (!(all & clearance) && !board.isSquareAttacked(travel, 2, WHITE, false)) {
                moves[count++] = Move(from, c8);
            }
        }
    }
}

void GenerateRookMoves(uint64_t pieces, Board& board, uint64_t friendly, uint64_t all, Move* moves, int& count, uint64_t limitMask = ~0ULL) {
    while (pieces) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pieces));
        pieces &= pieces -1;
        uint64_t pinRay = board.pinVersion[sq] == board.globalPinVersion ? board.pinMask[sq] : ~0ULL;

        uint64_t attacks = ROOK_ATTACKS(sq, all) & ~friendly & pinRay & limitMask;

        while (attacks) {
            uint8_t to = static_cast<uint8_t>(__builtin_ctzll(attacks));
            attacks &= attacks - 1;
            moves[count++] = Move(sq, to);
        }
    }
}

void GenerateBishopMoves(uint64_t pieces, Board& board, uint64_t friendly, uint64_t all, Move* moves, int& count, uint64_t limitMask = ~0ULL) {
    while (pieces) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pieces));
        pieces &= pieces -1;
        uint64_t pinRay = board.pinVersion[sq] == board.globalPinVersion ? board.pinMask[sq] : ~0ULL;

        uint64_t attacks = BISHOP_ATTACKS(sq, all) & ~friendly & pinRay & limitMask;

        while (attacks) {
            uint8_t to = static_cast<uint8_t>(__builtin_ctzll(attacks));
            attacks &= attacks - 1;
            moves[count++] = Move(sq, to);
        }
    }
}

void GenerateSlidingMoves(Board& board, uint64_t friendly, uint64_t all, Color side, Move* moves, int& count, uint64_t limitMask = ~0ULL) {
    GenerateRookMoves(board.bitboards[side][ROOK - 1], board, friendly, all, moves, count, limitMask);
    GenerateBishopMoves(board.bitboards[side][BISHOP - 1], board, friendly, all, moves, count, limitMask);

    GenerateRookMoves(board.bitboards[side][QUEEN - 1], board, friendly, all, moves, count, limitMask);
    GenerateBishopMoves(board.bitboards[side][QUEEN - 1], board, friendly, all, moves, count, limitMask);
}

void GenerateKnightMoves(uint64_t pieces, Board& board, uint64_t friendly, Move* moves, int& count, uint64_t limitMask = ~0ULL) {
    while(pieces) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pieces));
        pieces &= pieces - 1;
        if (board.pinVersion[sq] == board.globalPinVersion) continue;

        //Side does not matter here, it is symmetrical
        uint64_t attacks = attacksBB[KNIGHT - 1][WHITE][sq] & ~friendly & limitMask;

        while (attacks) {
            uint8_t to = static_cast<uint8_t>(__builtin_ctzll(attacks));
            attacks &= attacks - 1;
            moves[count++] = Move(sq, to);
        }
    }
}

void GenerateKingMoves(uint64_t pieces, Board& board, uint64_t friendly, Color side, Move* moves, int& count) {
    while(pieces) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pieces));
        pieces &= pieces - 1;

        uint64_t attacks = attacksBB[KING - 1][side][sq] & ~friendly;

        while (attacks) {
            uint8_t to = static_cast<uint8_t>(__builtin_ctzll(attacks));
            attacks &= attacks - 1;
            if (board.isSquareAttacked(to, static_cast<Color>(1-side), true)) continue;
            moves[count++] = Move(sq, to);
        }
    }
}

void GenerateNonSlidingMoves(Board& board, uint64_t friendly, Color side, Move* moves, int& count, uint64_t limitMask = ~0ULL) {
    GenerateKnightMoves(board.bitboards[side][KNIGHT - 1], board, friendly, moves, count, limitMask);
    GenerateKingMoves(board.bitboards[side][KING - 1], board, friendly, side, moves, count);
}

void GeneratePawnMoves(uint64_t pawns, Board& board, uint64_t all, uint64_t enemy, Color side, Move* moves, int& count, uint64_t limitMask = ~0ULL) {
    int8_t forward = 8 - (16 * side);
    uint8_t startRank = (side * 5) + 1;
    uint8_t promotionRank = 7 - startRank;

    uint64_t epTarget = board.hasEnPassant() ? board.getEnPassantTarget() : 0;

    while (pawns) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pawns));
        pawns &= pawns - 1;
        uint8_t rank = sq >> 3;
        bool canDouble = (rank == startRank);
        uint64_t pinRay = board.pinVersion[sq] == board.globalPinVersion ? board.pinMask[sq] : ~0ULL;
        limitMask |= epTarget == 0 ? 0 : bitMasks[epTarget];
        
        uint64_t attacks = attacksBB[PAWN-1][side][sq] & pinRay & limitMask;
        while (attacks) {
            uint8_t target = static_cast<uint8_t>(__builtin_ctzll(attacks));
            attacks &= attacks - 1;
            if (target == epTarget && epTarget != 0) {
                Move move = Move(sq, target, 0, true);
                board.MakeMove(move);
                if (!board.isKingInCheck(side == WHITE)) moves[count++] = move;
                board.UnmakeMove();
            } else if (bitMasks[target] & enemy) {
                if (rank == promotionRank) {
                    moves[count++] = Move(sq, target, 'q');
                    moves[count++] = Move(sq, target, 'r');
                    moves[count++] = Move(sq, target, 'b');
                    moves[count++] = Move(sq, target, 'n');
                }
                else moves[count++] = Move(sq, target);
            }
        }
        if (((sq + forward) < 0) || ((sq + forward) >= 64)) continue;
        uint8_t step = static_cast<uint8_t>(sq + forward);
        if ((bitMasks[step] & all) || !(bitMasks[step] & pinRay)) continue;
        if (bitMasks[step] & limitMask) {
            if (rank == promotionRank) {
                moves[count++] = Move(sq, step, 'q');
                moves[count++] = Move(sq, step, 'r');
                moves[count++] = Move(sq, step, 'b');
                moves[count++] = Move(sq, step, 'n');
            } else moves[count++] = Move(sq, step);
        }
        if (!canDouble) continue;
        step = static_cast<uint8_t>(step + forward);
        if (!(bitMasks[step] & limitMask)) continue;
        if (bitMasks[step] & all) continue;
        moves[count++] = Move(sq, step);
    }
}

int GenerateLegalMoves(Board& board, Move* moves /*Should be empty!*/) {
    int count = 0;
    Color side = static_cast<Color>(board.whiteToMove ? WHITE : BLACK);
    board.UpdatePins(side);
    uint64_t limitMask = ~0ULL;

    uint64_t friendly = board.generalboards[side];
    uint64_t enemy = board.generalboards[1 - side];
    uint64_t all = friendly | enemy;

    uint64_t checking = board.GetAttackersTo(board.kings[side], static_cast<Color>(1-side));
    uint8_t numOfCheckers = static_cast<uint8_t>(__builtin_popcountll(checking));
    if (numOfCheckers >= 2) {
        GenerateKingMoves(board.bitboards[side][KING-1], board, friendly, side, moves, count);
        return count;
    } else if (numOfCheckers == 1) {
        uint8_t attackerSquare = static_cast<uint8_t>(__builtin_ctzll(checking));
        limitMask = lineBB[attackerSquare][board.kings[side]] | bitMasks[attackerSquare];
    }

    GenerateSlidingMoves(board, friendly, all, side, moves, count, limitMask);
    GenerateNonSlidingMoves(board, friendly, side, moves, count, limitMask);
    GeneratePawnMoves(board.bitboards[side][PAWN-1], board, all, enemy, side, moves, count, limitMask);

    GenerateCastlingMoves(board, all, moves, side, count);

    return count;
}

int GenerateCaptures(Board& board, Move* moves) {
    int count = 0;
    (void) board;
    (void) moves;
    (void) count;
    //TODO
    return 0;
}