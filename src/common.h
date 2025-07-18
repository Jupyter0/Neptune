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
#include <thread>
#include <atomic>
#include <mutex>

extern uint numThreads;

#define ROOK_ATTACKS(sq, occ) (rookAttackTable[sq][((occ & rookBlockerMask[sq]) * rookMagic[sq]) >> rookShift[sq]])
#define BISHOP_ATTACKS(sq, occ) (bishopAttackTable[sq][((occ & bishopBlockerMask[sq]) * bishopMagic[sq]) >> bishopShift[sq]])

extern uint64_t zobristPiece[2][6][64];  // [color][pieceType][square]
extern uint64_t zobristEnPassant[8];    // [file]
extern uint64_t zobristCastling[16];    // 4-bit castling rights (16 possibilities)
extern uint64_t zobristSideToMove;      // Just one value

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

extern uint64_t fileA;
extern uint64_t fileB;
extern uint64_t fileC;
extern uint64_t fileD;
extern uint64_t fileE;
extern uint64_t fileF;
extern uint64_t fileG;
extern uint64_t fileH;

extern int infinity;

extern std::array<std::array<uint64_t, 64>, 64> castleXOR;
extern uint64_t lineBB[64][64];

namespace NeptuneInternals
{
    enum Squares : uint8_t{
        a1, b1, c1, d1, e1, f1, g1, h1,
        a2, b2, c2, d2, e2, f2, g2, h2,
        a3, b3, c3, d3, e3, f3, g3, h3,
        a4, b4, c4, d4, e4, f4, g4, h4,
        a5, b5, c5, d5, e5, f5, g5, h5,
        a6, b6, c6, d6, e6, f6, g6, h6,
        a7, b7, c7, d7, e7, f7, g7, h7,
        a8, b8, c8, d8, e8, f8, g8, h8
    };
    
    enum Piece : uint8_t {
        EMPTY, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
    };

    enum Color : uint8_t{
        WHITE, BLACK
    };

    struct MoveState {
        private:
            uint64_t stateHigh;
            uint64_t stateLow;
        public:
        MoveState() : stateHigh(0), stateLow(0) {}
        /**
         * @brief Only the last 4 bits will be set.
         * 
         * Data is packed bits 60-63 of stateHigh
         */
        void SetCastlingRights(uint8_t castlingRights) {
            uint8_t data = castlingRights & 0xF;
            stateHigh &= ~(0xFULL << 60);
            stateHigh |= static_cast<uint64_t>(data) << 60;
        }
        /**
         * @brief Only the last 10 bits will be set.
         * 
         * Max move count of 1023
         * 
         * Data is packed in bits 50-59 of stateHigh
         */
        void SetFullMoveNumber(uint16_t fullMoveNumber) {
            uint16_t data = fullMoveNumber & 0x3FF;
            stateHigh &= ~(0x3FFULL << 50);
            stateHigh |= static_cast<uint64_t>(data) << 50;
        }
        /**
         * @brief Data is packed in bit 49 of stateHigh
         */
        void SetIsEnpassant(bool isEnpassant) {
            uint8_t data = static_cast<uint8_t>(isEnpassant);
            stateHigh &= ~(0x1ULL << 49);
            stateHigh |= static_cast<uint64_t>(data) << 49;
        }
        /**
         * @brief Only the last 7 bits will be set
         * 
         * En passant data is packed as the following:
         * 
         * //0bfdddddd f: flag, d: data
         * 
         * f -> Whether EP is possible
         * 
         * d -> If EP is possible, which square
         * 
         * Data is packed in bits 42-48 of stateHigh
         */
        void SetEnPassantTarget(uint8_t enPassantTarget) {
            uint8_t data = enPassantTarget & 0x7F;
            stateHigh &= ~(0x7FULL << 42);
            stateHigh |= static_cast<uint64_t>(data) << 42;
        }
        /**
         * @brief If casting from uint, verify that piece <= 6
         * 
         * Only last 3 bits will be set
         * 
         * Data is packed in bits 39-41 of stateHigh
         */
        void SetCapturedPiece(Piece piece) {
            uint8_t data = piece & 0x7;
            stateHigh &= ~(0x7ULL << 39);
            stateHigh |= static_cast<uint64_t>(data) << 39;
        }
        /**
         * @brief If casting from uint, verify that piece <= 6
         * 
         * Only last 3 bits will be set
         * 
         * Data is packed in bits 36-38 of stateHigh
         */
        void SetMovedPiece(Piece piece) {
            uint8_t data = piece & 0x7;
            stateHigh &= ~(0x7ULL << 36);
            stateHigh |= static_cast<uint64_t>(data) << 36;
        }
        /**
         * @brief If casting from uint, verify that piece <= 6
         * 
         * Only last 3 bits will be set
         * 
         * Data is packed in bits 33-35 of stateHigh
         */
        void SetPromotedPiece(Piece piece) {
            uint8_t data = piece & 0x7;
            stateHigh &= ~(0x7ULL << 33);
            stateHigh |= static_cast<uint64_t>(data) << 33;
        }
        /**
         * @brief Only last 6 bits from each are set
         * 
         * Data is packed in bits 21-32 of stateHigh
         */
        void SetFromTo(uint8_t from, uint8_t to) {
            uint16_t data = static_cast<uint16_t>(((from & 0x3F) << 6U) | (to & 0x3F));
            stateHigh &= ~(0xFFFULL << 21);
            stateHigh |= static_cast<uint64_t>(data) << 21;
        }
        /**
         * @brief Data is packed in bit 20 of stateHigh
         */
        void SetIsWhiteToMove(bool isWhiteToMove) {
            uint8_t data = static_cast<uint8_t>(isWhiteToMove);
            stateHigh &= ~(0x1ULL << 20);
            stateHigh |= static_cast<uint64_t>(data) << 20;
        }
        /**
         * @brief All 8 bits can be used.
         * 
         * Should be used for 50 move rule.
         * 
         * Data is packed in bits 12-19 of stateHigh
         */
        void SetHalfmoveClock(uint8_t halfmoveClock) {
            stateHigh &= ~(0xFFULL << 12);
            stateHigh |= static_cast<uint64_t>(halfmoveClock) << 12;
        }
        /**
         * @brief Data is packed in bits 0-63 of stateLow
         */
        void SetZobristKey(uint64_t zobristKey) {
            stateLow = zobristKey;
        }
        
        /**
         * @brief Data returned is in the lower 4 bits
         */
        uint8_t GetCastlingRights() {
            return static_cast<uint8_t>((stateHigh >> 60) & 0xF);
        }
        /**
         * @brief Data returned is in the lower 10 bits
         */
        uint16_t GetFullMoveNumber() {
            return static_cast<uint16_t>((stateHigh >> 50) & 0x3FF);
        }
        bool GetIsEnpassant() {
            return static_cast<uint8_t>((stateHigh >> 49) & 0x1);
        }
        /**
         * @brief Data returned is in the lower 7 bits
         * 
         * En passant data is packed as the following:
         * 
         * 0bfdddddd f: flag, d: data
         * 
         * f -> Whether EP is possible
         * 
         * d -> If EP is possible, which square
         */
        uint8_t GetEnPassantTarget() {
            return static_cast<uint8_t>((stateHigh >> 42) & 0x7F);
        }
        Piece GetCapturedPiece(){
            return static_cast<Piece>((stateHigh >> 39) & 0x7);
        }
        Piece GetMovedPiece() {
            return static_cast<Piece>((stateHigh >> 36) & 0x7);
        }
        Piece GetPromotedPiece() {
            return static_cast<Piece>((stateHigh >> 33) & 0x7);
        }
        /**
         * @brief Data is @brief Data returned is in the lower 6 bits
         */
        uint8_t GetFrom() {
            return static_cast<uint8_t>((stateHigh >> 27) & 0x3F);
        }
        /**
         * @brief Data returned is in the lower 4 bits
         */
        uint8_t GetTo() {
            return static_cast<uint8_t>((stateHigh >> 21) & 0x3F);
        }
        bool GetIsWhiteToMove() {
            return static_cast<uint8_t>((stateHigh >> 20) & 0x1);
        }
        /**
         * @brief Data returned is in all bits
         */
        uint8_t GetHalfMoveClock() {
            return static_cast<uint8_t>((stateHigh >> 12) & 0xFF);
        }
        /**
         * @brief Data returned is in all bits
         */
        uint64_t GetZobristKey() {
            return stateLow;
        }
    };

    struct Move {
        uint8_t from;
        uint8_t to;
        char promotion = 0;
        bool isEnPassant;

        Move(uint8_t From, uint8_t To, char Promotion = 0, bool EP = false) : from(From), to(To), promotion(Promotion), isEnPassant(EP) {}
        Move() : from(0), to(0) {}

        bool operator==(const Move& other) const {
            return from == other.from &&
                to == other.to &&
                promotion == other.promotion &&
                isEnPassant == other.isEnPassant;
        }
    };

    inline constexpr std::array<Piece, 128> charToPiece = []() {
        std::array<Piece, 128> table{};

        table['p'] = PAWN;
        table['n'] = KNIGHT;
        table['b'] = BISHOP;
        table['r'] = ROOK;
        table['q'] = QUEEN;
        table['k'] = KING;

        return table;
    }();
}

std::string IndexToFileRank(uint8_t index);

NeptuneInternals::Move ParseUCIMove(const std::string& uci, NeptuneInternals::Piece pieceTable[64]);

std::vector<std::string> SplitFen(const std::string& s);

std::string MoveToUCI(const NeptuneInternals::Move& move);

void InitLineBB();

#endif