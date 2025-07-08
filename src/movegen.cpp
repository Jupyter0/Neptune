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

void GenerateRookMoves(uint64_t pieces, Board& board, Color color, std::vector<Move>& moves) {
    const uint64_t all = board.generalboards[2];
    const uint64_t friendly = board.generalboards[color];
    while (pieces) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pieces));
        pieces &= pieces -1;

        uint64_t attacks = ROOK_ATTACKS(sq, all) & ~friendly;

        while (attacks) {
            uint8_t to = static_cast<uint8_t>(__builtin_ctzll(attacks));
            attacks &= attacks - 1;
            moves.push_back(Move(sq, to));
        }
    }
}

void GenerateBishopMoves(uint64_t pieces, Board& board, Color color, std::vector<Move>& moves) {
    const uint64_t all = board.generalboards[2];
    const uint64_t friendly = board.generalboards[color];
    while (pieces) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pieces));
        pieces &= pieces -1;

        uint64_t attacks = BISHOP_ATTACKS(sq, all) & ~friendly;

        while (attacks) {
            uint8_t to = static_cast<uint8_t>(__builtin_ctzll(attacks));
            attacks &= attacks - 1;
            moves.push_back(Move(sq, to));
        }
    }
}

void GenerateSlidingMoves(Board& board, Color color, std::vector<Move>& moves) {
    GenerateRookMoves(board.bitboards[color][3], board, color, moves);
    GenerateBishopMoves(board.bitboards[color][2], board, color, moves);

    GenerateRookMoves(board.bitboards[color][4], board, color, moves);
    GenerateBishopMoves(board.bitboards[color][4], board, color, moves);
}

void GenerateKnightMoves(uint64_t pieces, Board& board, Color color, std::vector<Move>& moves) {
    const uint64_t friendly = board.generalboards[color];
    while(pieces) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pieces));
        pieces &= pieces - 1;

        uint64_t attacks = attacksBB[KNIGHT-1][color][sq] & ~friendly;

        while (attacks) {
            uint8_t to = static_cast<uint8_t>(__builtin_ctzll(attacks));
            attacks &= attacks - 1;
            moves.push_back(Move(sq, to));
        }
    }
}

void GenerateKingMoves(uint64_t pieces, Board& board, Color color, std::vector<Move>& moves) {
    const uint64_t friendly = board.generalboards[color];
    while(pieces) {
        uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(pieces));
        pieces &= pieces - 1;

        uint64_t attacks = attacksBB[KING-1][color][sq] & ~friendly;

        while (attacks) {
            uint8_t to = static_cast<uint8_t>(__builtin_ctzll(attacks));
            attacks &= attacks - 1;
            moves.push_back(Move(sq, to));
        }
    }
}

void GenerateNonSlidingMoves(Board& board, Color color, std::vector<Move>& moves) {
    GenerateKnightMoves(board.bitboards[color][1], board, color, moves);
    GenerateKingMoves(board.bitboards[color][5], board, color, moves);
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

    GenerateSlidingMoves(board, color, moves);
    GenerateNonSlidingMoves(board, color, moves);
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