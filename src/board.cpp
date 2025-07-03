#include <cstdint>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include "board.h"


void Board::UpdateAttacks() {
    whiteAttacks = 0ULL;
    blackAttacks = 0ULL;

    AddNonSlidingAttacks(whiteKnights, KNIGHT, WHITE, whiteAttacks);
    AddNonSlidingAttacks(whiteKing, KING, WHITE, whiteAttacks);
    AddNonSlidingAttacks(whitePawns, PAWN, WHITE, whiteAttacks);
    AddNonSlidingAttacks(blackKnights, KNIGHT, BLACK, blackAttacks);
    AddNonSlidingAttacks(blackKing, KING, BLACK, blackAttacks);
    AddNonSlidingAttacks(blackPawns, PAWN, BLACK, blackAttacks);

    AddSlidingAttacks(whiteBishops, BISHOP, WHITE, whiteAttacks);
    AddSlidingAttacks(whiteRooks, ROOK, WHITE, whiteAttacks);
    AddSlidingAttacks(whiteQueens, QUEEN, WHITE, whiteAttacks);
    AddSlidingAttacks(blackBishops, BISHOP, BLACK, blackAttacks);
    AddSlidingAttacks(blackRooks, ROOK, BLACK, blackAttacks);
    AddSlidingAttacks(blackQueens, QUEEN, BLACK, blackAttacks);
}
void Board::AddSlidingAttacks(uint64_t pieces, Piece pieceType, Color color, uint64_t& attacks) {
    if (pieceType == KNIGHT || pieceType == KING || pieceType == PAWN) return;
    if (pieceType == QUEEN) {
        AddSlidingAttacks(pieces, ROOK, color, attacks);
        AddSlidingAttacks(pieces, BISHOP, color, attacks);
        return;
    }
    uint64_t p = pieces;
    while(p) {
        int sq = __builtin_ctzll(p);
        p &= p - 1;

        switch (pieceType) {
            case BISHOP: {
                attacks |= CalculateSlidingAttacks(sq, bishopDirs, allPieces);
                break;
            }
            case ROOK: {
                attacks |= CalculateSlidingAttacks(sq, rookDirs, allPieces);
                break;
            }
            default: break;
        }
    }
}

uint64_t Board::CalculateSlidingAttacks(uint64_t sq, const int directions[][2], uint64_t all) {
    int rank = sq >> 3;
    int file = sq & 7;
    uint64_t attacks = 0;

    for (int d = 0; d < 4; d++) {
        int r = rank + directions[d][0];
        int f = file + directions[d][1];
        while (r >= 0 && r < 8 && f >= 0 && f < 8) {
            int target = r * 8 + f;
            uint64_t targetBB = bitMasks[target];
            attacks |= targetBB;
            if (all & targetBB) break;
            r += directions[d][0];
            f += directions[d][1];
        }           
    }
    return attacks;
}

void Board::AddNonSlidingAttacks(uint64_t pieces, Piece pieceType, Color color, uint64_t& attacks) {
    if (pieceType == BISHOP || pieceType == ROOK || pieceType == QUEEN) return;
    uint64_t p = pieces;
    while(p) {
        int sq = __builtin_ctzll(p);
        p &= p - 1;

        switch (pieceType) {
            case KNIGHT: attacks |= knightAttacks[sq]; break;
            case KING: attacks |= kingAttacks[sq]; break;
            case PAWN: attacks |= pawnAttacks[color][sq]; break;
            default: break;
        }
    }
}

void Board::make_move(Move move) {
    int from = move.from;
    int to = move.to;
    bool isWhite = whiteToMove;
    uint64_t fromBB = bitMasks[from];
    uint64_t toBB = bitMasks[to];
    enPassantSquare = 0;
    halfmoveClock++;
    Piece movedPiece = pieceAt[from];
    Piece capturedPiece = pieceAt[to];
    pieceAt[to] = movedPiece;

    uint64_t* bitboards[2][6] = {
        { &whitePawns, &whiteKnights, &whiteBishops, &whiteRooks, &whiteQueens, &whiteKing },
        { &blackPawns, &blackKnights, &blackBishops, &blackRooks, &blackQueens, &blackKing }
    };

    Color mySide = isWhite ? WHITE : BLACK;

    if (movedPiece == PAWN) {
        *bitboards[mySide][0] &= ~fromBB;
        *bitboards[mySide][0] |= toBB;
        halfmoveClock = 0;
        if (move.isEnPassant) {
            int capSq = isWhite ? to - 8 : to + 8;
            *bitboards[!isWhite][0] &= ~bitMasks[capSq];
            pieceAt[capSq] = EMPTY;
        }
        if ((fromBB & rank2) && (toBB & rank4)) {
            enPassantSquare = (1 << 6) | (from + 8);
        }
        if (move.promotion != 0) {
            *bitboards[mySide][0] &= ~toBB;
            Piece promoted = charToPiece(move.promotion);
            *bitboards[mySide][promoted-1] |= toBB;
            pieceAt[to] = promoted;
        }
    } else if (movedPiece == KING) {
        *bitboards[mySide][5] &= ~fromBB;
        *bitboards[mySide][5] |= toBB;
        if (mySide == WHITE) whiteKingPos = to;
        else blackKingPos = to;

        if (from == e1 || from == e8) {
            castlingRights &= ~(3 << (2*(1-mySide)));
            if (to == g1) {
                whiteRooks = (whiteRooks & ~bitMasks[h1]) | bitMasks[f1];
                pieceAt[h1] = EMPTY;
                pieceAt[f1] = ROOK;
            } else if (to == c1) {
                whiteRooks = (whiteRooks & ~bitMasks[a1]) | bitMasks[d1];
                pieceAt[a1] = EMPTY;
                pieceAt[d1] = ROOK;
            } else if (to == g8) {
                blackRooks = (blackRooks & ~bitMasks[h8]) | bitMasks[f8];
                pieceAt[h8] = EMPTY;
                pieceAt[f8] = ROOK;
            } else if (to == c8) {
                blackRooks = (blackRooks & ~bitMasks[a8]) | bitMasks[d8];
                pieceAt[a8] = EMPTY;
                pieceAt[d8] = ROOK;
            }
        }

    } else {
        *bitboards[mySide][movedPiece-1] &= ~fromBB;
        *bitboards[mySide][movedPiece-1] |= toBB;
    }

    castlingRights &= ~(castlingClearTable[from] | castlingClearTable[to]);

    if (capturedPiece != EMPTY) {
        *bitboards[1-mySide][capturedPiece-1] &= ~toBB;
        halfmoveClock = 0;
    }

    pieceAt[from] = EMPTY;

    whiteToMove = !whiteToMove;
    if (!whiteToMove) fullmoveNumber++;

    UpdateOccupancy();
    UpdateAttacks();
}
bool Board::is_king_in_check(bool white) {
    return white ? (blackAttacks & bitMasks[whiteKingPos]) != 0
                : (whiteAttacks & bitMasks[blackKingPos]) != 0;
}
void Board::UpdateOccupancy() {
    whitePieces = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing;
    blackPieces = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing;
    allPieces = whitePieces | blackPieces;

    whiteKingPos = __builtin_ctzll(whiteKing);
    blackKingPos = __builtin_ctzll(blackKing);
}
bool Board::hasEnPassant() const {
    return (enPassantSquare & (1 << 6)) != 0;
}
uint8_t Board::getEnPassantTarget() const {
    // Returns 0–63 square index if en passant is available; undefined otherwise
    return enPassantSquare & 0b00111111;
}