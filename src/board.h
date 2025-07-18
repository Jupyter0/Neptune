#ifndef BOARD_H
#define BOARD_H

#include "common.h"
#include "magicbitboards.h"

using namespace NeptuneInternals;

class Board {
public:
    MoveState history[256];
    int ply = 0;

    uint64_t pinMask[64] = {~0ULL};
    uint64_t pinVersion[64] = {0};
    uint64_t globalPinVersion = 0;

    uint8_t kings[2];

    uint64_t whiteAttacks;
    uint64_t blackAttacks;

    uint64_t bitboards[2][6]; //Pawn = 0, Knight = 1, Bishop = 2, Rook = 3, Queen = 4, King = 5

    uint64_t generalboards[3];

    bool whiteToMove;

    uint8_t castlingRights; //0b0000KQkq K: WKS, Q: WQS, k: BKS, q: BQS
    uint8_t enPassantSquare; //0b0fdddddd f: flag, d: data

    uint8_t halfmoveClock; //Used for 50-move rule
    uint16_t fullmoveNumber; //Counts the move number

    uint64_t zobristKey = 0; //Zobrist Hash !!Don't use this, this feature is not yet implemented!!

    Piece pieceAt[64] = {EMPTY};


    std::vector<Move> appliedMoves;
    void MakeMove(Move move);
    void UnmakeMove();
    void UpdateOccupancy();
    bool hasEnPassant() const;
    bool isKingInCheck(bool white);
    bool isSquareAttacked(uint8_t square, Color attacker, bool excludeKing);
    bool isSquareAttacked(uint8_t* squares, int numSquares, Color attacker, bool excludeKing);
    uint8_t getEnPassantTarget() const;
    uint64_t GetAttackersTo(uint8_t square, Color attackingSide);
    void UpdatePins(Color forSide);
    void SaveSnapshot(Move move);
};

#endif