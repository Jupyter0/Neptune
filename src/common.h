#ifndef COMMON_H
#define COMMON_H

#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <chrono>
#include <cstring>

#define ROOK_ATTACKS(sq, occ) (rookAttackTable[sq][((occ & rookBlockerMask[sq]) * rookMagic[sq]) >> rookShift[sq]])
#define BISHOP_ATTACKS(sq, occ) (bishopAttackTable[sq][((occ & bishopBlockerMask[sq]) * bishopMagic[sq]) >> bishopShift[sq]])

enum Squares {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};

enum Piece {
    EMPTY, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};

enum Color {
    WHITE, BLACK
};

struct MoveState {
    uint8_t castlingRights; //0b0000KQkq K: WKS, Q: WQS, k: BKS, q: BQS
    uint8_t enPassantSquare; //0b0fdddddd f: flag, d: data
    int halfmoveClock; //Used for 50-move rule
    int fullmoveNumber; //Counts the move number

    Piece capturedPiece;
    Piece movedPiece;
    Piece promotedPiece;
    bool enPassant;

    uint8_t from, to;

    bool whiteToMove;

    uint8_t whiteKingPos, blackKingPos;
};

struct Move {
    uint8_t from;
    uint8_t to;
    char promotion = 0;
    bool isEnPassant;

    Move(uint8_t From, uint8_t To, char Promotion = 0, bool EP = false) : from(From), to(To), promotion(Promotion), isEnPassant(EP) {}
};

/**
 * @brief Bitboard array for piece attacks
 * @attention Entries for sliding pieces is empty. They are {} past the first index
 * 
 * attacksBB[Piece][Color][Square]
 * 
 * Piece: 0 = Pawn, 1 = Knight, ..., 5 = King
 * 
 * Color: 0 = White, 1 = Black
 * 
 * Square: (0-63) (A1-H8)
 */
extern uint64_t attacksBB[6][2][64];

extern uint8_t castlingClearTable[64];

extern uint64_t bitMasks[64];

extern uint64_t castlingBB[4];

extern uint64_t rank1;
extern uint64_t rank2;
extern uint64_t rank3;
extern uint64_t rank4;
extern uint64_t rank5;
extern uint64_t rank6;
extern uint64_t rank7;
extern uint64_t rank8;

extern std::array<Piece, 128> promotionCharToPiece;

std::string indexToSquare(uint8_t index);

Piece charToPiece(char pieceChar);

Move parseUCIMove(const std::string& uci);

std::vector<std::string> split(const std::string& s);

std::string MoveToUCI(const Move& move);

#endif