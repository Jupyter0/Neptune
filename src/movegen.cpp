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
        int sq = __builtin_ctzll(pieces);
        pieces &= pieces - 1;

        int rank = sq >> 3;
        int file = sq & 7;

        for (int d = 0; d < dirCount; d++) {
            int r = rank + directions[d][0];
            int f = file + directions[d][1];
            while (r >= 0 && r < 8 && f >= 0 && f < 8) {
                int target = r * 8 + f;
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
        int sq = __builtin_ctzll(pieces);
        pieces &= pieces - 1;

        uint64_t attacks = attackTable[sq];
        attacks &= ~own;

        while (attacks) {
            int target = __builtin_ctzll(attacks);
            attacks &= attacks - 1;
            moves.push_back(Move(sq, target));
        }
    }
}


void GeneratePawnMoves(uint64_t pawns, uint64_t enemy, uint64_t all, std::vector<Move>& moves, Color color, uint8_t epSquare = 64) {
    int forward = (color == WHITE) ? 8 : -8;
    int startRank = (color == WHITE) ? 1 : 6;
    int promotionRank = (color == WHITE) ? 6 : 1;

    while (pawns) {
        int sq = __builtin_ctzll(pawns);
        pawns &= pawns - 1;

        int rank = sq >> 3;

        int oneStep = sq + forward;
        if (oneStep >= 0 && oneStep < 64 && !(all & bitMasks[oneStep])) {
            if (rank == promotionRank) {
                moves.push_back(Move(sq, oneStep, 'q'));
                moves.push_back(Move(sq, oneStep, 'r'));
                moves.push_back(Move(sq, oneStep, 'b'));
                moves.push_back(Move(sq, oneStep, 'n'));
            } else {
                moves.push_back(Move(sq, oneStep));

                if (rank == startRank) {
                    int twoStep = sq + 2 * forward;
                    if (!(all & bitMasks[twoStep])) {
                        moves.push_back(Move(sq, twoStep));
                    }
                }
            }
        }

        uint64_t attacks = pawnAttacks[color][sq];
        uint64_t normalCaptures = attacks & enemy;

        while (normalCaptures) {
            int targetSq = __builtin_ctzll(normalCaptures);
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

        if (epSquare < 64) {
            uint64_t epBB = bitMasks[epSquare];
            if ((attacks & epBB) != 0) {
                moves.push_back(Move(sq, epSquare, 0, true));
            }
        }
    }
}

std::vector<Move> GeneratePseudoLegalMoves(Board& board, bool isWhiteToMove) {
    std::vector<Move> moves;
    moves.reserve(256);
    uint8_t epTarget = board.hasEnPassant() ? board.getEnPassantTarget() : 64;

    if (isWhiteToMove) {
        GenerateSlidingMoves(board.whiteBishops, bishopDirs, std::size(bishopDirs), board.whitePieces, board.blackPieces, moves);
        GenerateSlidingMoves(board.whiteRooks,   rookDirs,   std::size(rookDirs),   board.whitePieces, board.blackPieces, moves);
        GenerateSlidingMoves(board.whiteQueens,  queenDirs,  std::size(queenDirs),  board.whitePieces, board.blackPieces, moves);
        GenerateNonSlidingMoves(board.whiteKnights, knightAttacks, board.whitePieces, moves);
        GenerateNonSlidingMoves(board.whiteKing, kingAttacks, board.whitePieces, moves);
        GeneratePawnMoves(board.whitePawns, board.blackPieces, board.allPieces, moves, WHITE, epTarget);
        if (!board.is_king_in_check(true)) GenerateCastlingMoves(board.castlingRights, board.allPieces, moves, WHITE, board.whiteAttacks, board.blackAttacks);
    } else {
        GenerateSlidingMoves(board.blackBishops, bishopDirs, std::size(bishopDirs), board.blackPieces, board.whitePieces, moves);
        GenerateSlidingMoves(board.blackRooks,   rookDirs,   std::size(rookDirs),   board.blackPieces, board.whitePieces, moves);
        GenerateSlidingMoves(board.blackQueens,  queenDirs,  std::size(queenDirs),  board.blackPieces, board.whitePieces, moves);
        GenerateNonSlidingMoves(board.blackKnights, knightAttacks, board.blackPieces, moves);
        GenerateNonSlidingMoves(board.blackKing, kingAttacks, board.blackPieces, moves);
        GeneratePawnMoves(board.blackPawns, board.whitePieces, board.allPieces, moves, BLACK, epTarget);
        if (!board.is_king_in_check(false)) GenerateCastlingMoves(board.castlingRights, board.allPieces, moves, BLACK, board.whiteAttacks, board.blackAttacks);
    }

    return moves;
};

std::vector<Move> GenerateLegalMoves(Board& board) {
    std::vector<Move> pseudoMoves = GeneratePseudoLegalMoves(board, board.whiteToMove);
    std::vector<Move> legalMoves;
    legalMoves.reserve(pseudoMoves.size());

    for (const Move& move : pseudoMoves) {
        Board temp = board;
        temp.make_move(move);
        if (!temp.is_king_in_check(!temp.whiteToMove)) { // check if own king is not in check
            legalMoves.push_back(move);
        }
    }

    return legalMoves;
}