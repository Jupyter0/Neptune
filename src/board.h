#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include "common.h"
#include <string>

class Board {
public:
    std::vector<MoveState> history;

    uint64_t whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing;
    uint64_t blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing;

    uint64_t whitePieces;
    uint64_t blackPieces;
    uint64_t allPieces;

    uint8_t whiteKingPos;
    uint8_t blackKingPos;

    uint64_t whiteAttacks;
    uint64_t blackAttacks;

    uint64_t* bitboards[2][6] = {
        { &whitePawns, &whiteKnights, &whiteBishops, &whiteRooks, &whiteQueens, &whiteKing },
        { &blackPawns, &blackKnights, &blackBishops, &blackRooks, &blackQueens, &blackKing }
    };

    bool whiteToMove;

    uint8_t castlingRights; //0b0000KQkq K: WKS, Q: WQS, k: BKS, q: BQS
    uint8_t enPassantSquare; //0b0fdddddd f: flag, d: data

    int halfmoveClock; //Used for 50-move rule
    int fullmoveNumber; //Counts the move number

    uint64_t zobristKey = 0; //Zobrist Hash !!Don't use this, this feature is not yet implemented!!

    Piece pieceAt[64] = {EMPTY};


    std::vector<Move> appliedMoves;

    void UpdateAttacks();
    void AddSlidingAttacks(uint64_t pieces, Piece pieceType, Color color, uint64_t& attacks);
    uint64_t CalculateSlidingAttacks(uint64_t sq, const int directions[][2], uint64_t all);
    void AddNonSlidingAttacks(uint64_t pieces, Piece pieceType, Color color, uint64_t& attacks);
    void make_move(Move move);
    void unmake_move();
    MoveState getSnapshot() const;
    bool is_king_in_check(bool white);
    void UpdateOccupancy();
    bool hasEnPassant() const;
    uint8_t getEnPassantTarget() const;
};

#endif