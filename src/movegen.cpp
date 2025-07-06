#include "movegen.h"
#include "board.h"

void GenerateCastlingMoves(uint8_t castlingRights, uint64_t all, std::vector<Move>& moves, Color color, uint64_t whiteAttacks, uint64_t blackAttacks) {
    if (color == WHITE) {
        if (((castlingRights & 0b1000) != 0) && ((all & castlingBB[0]) == 0) && ((blackAttacks & bitMasks[f1]) == 0)) {
            moves.push_back(Move(e1, g1));
        }
        if (((castlingRights & 0b0100) != 0) && ((all & castlingBB[1]) == 0) && ((blackAttacks & bitMasks[d1]) == 0) && ((blackAttacks & bitMasks[c1]) == 0)) {
            moves.push_back(Move(e1, c1));
        }
    } else {
        if (((castlingRights & 0b0010) != 0) && ((all & castlingBB[2]) == 0) && ((whiteAttacks & bitMasks[f8]) == 0)) {
            moves.push_back(Move(e8, g8));
        }
        if (((castlingRights & 0b0001) != 0) && ((all & castlingBB[3]) == 0) && ((whiteAttacks & bitMasks[d8]) == 0) && ((whiteAttacks & bitMasks[c8]) == 0)) {
            moves.push_back(Move(e8, c8));
        }
    }
}

void GenerateSlidingMoves(uint64_t pieces, const int directions[][2], int dirCount, uint64_t own, uint64_t enemy, std::vector<Move>& moves) {
    while (pieces) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pieces));
        pieces &= pieces - 1;

        uint8_t rank = sq >> 3;
        uint8_t file = sq & 7;

        for (int d = 0; d < dirCount; d++) {
            int r = rank + directions[d][0];
            int f = file + directions[d][1];
            while (r >= 0 && r < 8 && f >= 0 && f < 8) {
                uint8_t target = static_cast<uint8_t>(r * 8 + f);
                uint64_t targetBB = bitMasks[target];
                if (own & targetBB) break;
                moves.push_back(Move(sq, target));
                if (enemy & targetBB) break;
                r += directions[d][0];
                f += directions[d][1];
            }
        }
    }
}

void GenerateNonSlidingMoves(uint64_t pieces, const uint64_t attackTable[64], uint64_t own, std::vector<Move>& moves) {
    while (pieces) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pieces));
        pieces &= pieces - 1;

        uint64_t attacks = attackTable[sq];
        attacks &= ~own;

        while (attacks) {
            uint8_t target = static_cast<uint8_t>(__builtin_ctzll(attacks));
            attacks &= attacks - 1;
            moves.push_back(Move(sq, target));
        }
    }
}


void GeneratePawnMoves(uint64_t pawns, uint64_t enemy, uint64_t all, std::vector<Move>& moves, Color color, uint8_t epSquare = 0) {
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
                moves.push_back(Move(sq, oneStep, 'q'));
                moves.push_back(Move(sq, oneStep, 'r'));
                moves.push_back(Move(sq, oneStep, 'b'));
                moves.push_back(Move(sq, oneStep, 'n'));
            } else {
                moves.push_back(Move(sq, oneStep));

                if (rank == startRank) {
                    uint8_t twoStep = static_cast<uint8_t>(sq + 2 * forward);
                    if (!(all & bitMasks[twoStep]) && !(all & bitMasks[oneStep])) {
                        moves.push_back(Move(sq, twoStep));
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
                moves.push_back(Move(sq, targetSq, 'q'));
                moves.push_back(Move(sq, targetSq, 'r'));
                moves.push_back(Move(sq, targetSq, 'b'));
                moves.push_back(Move(sq, targetSq, 'n'));
            } else {
                moves.push_back(Move(sq, targetSq));
            }
        }

        if (epSquare != 0) {
            uint64_t epBB = bitMasks[epSquare];
            if ((attacks & epBB) != 0) {
                moves.push_back(Move(static_cast<uint8_t>(sq), epSquare, 0, true));
            }
        }
    }
}

std::vector<Move> GeneratePseudoLegalMoves(Board& board, bool isWhiteToMove) {
    std::vector<Move> moves;
    moves.reserve(256);
    uint8_t epTarget = board.hasEnPassant() ? board.getEnPassantTarget() : 0;
    Color color = static_cast<Color>(isWhiteToMove ? WHITE : BLACK);

    GenerateSlidingMoves(board.bitboards[color][BISHOP-1], dirs[BISHOP-1], 4, board.generalboards[color], board.generalboards[1-color], moves);
    GenerateSlidingMoves(board.bitboards[color][ROOK-1], dirs[ROOK-1], 4, board.generalboards[color], board.generalboards[1-color], moves);
    GenerateSlidingMoves(board.bitboards[color][QUEEN-1], dirs[QUEEN-1], 8, board.generalboards[color], board.generalboards[1-color], moves);

    GenerateNonSlidingMoves(board.bitboards[color][KNIGHT-1], attacksBB[KNIGHT-1][color], board.generalboards[color], moves);
    GenerateNonSlidingMoves(board.bitboards[color][KING-1], attacksBB[KING-1][color], board.generalboards[color], moves);
    
    GeneratePawnMoves(board.bitboards[color][PAWN-1], board.generalboards[1-color], board.generalboards[2], moves, color, epTarget);

    if (!board.is_king_in_check(isWhiteToMove)) GenerateCastlingMoves(board.castlingRights, board.generalboards[2], moves, color, board.whiteAttacks, board.blackAttacks);

    return moves;
};

std::vector<Move> GenerateLegalMoves(Board& board) {
    std::vector<Move> pseudoMoves = GeneratePseudoLegalMoves(board, board.whiteToMove);
    std::vector<Move> legalMoves;
    legalMoves.reserve(pseudoMoves.size());

    for (const Move& move : pseudoMoves) {
        board.make_move(move);
        if (!board.is_king_in_check(!board.whiteToMove)) { // check if own king is not in check
            legalMoves.push_back(move);
        }
        board.unmake_move();
    }

    return legalMoves;
}