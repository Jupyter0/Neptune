#include "board.h"
#include "attackgen.h"

void Board::make_move(Move move) {
    uint8_t from = move.from;
    uint8_t to = move.to;
    bool isWhite = whiteToMove;
    uint64_t fromBB = bitMasks[from];
    uint64_t toBB = bitMasks[to];
    Piece movedPiece = pieceAt[from];
    Piece capturedPiece = pieceAt[to];
    
    MoveState snapshot;
    snapshot.castlingRights = castlingRights;
    snapshot.enPassantSquare = enPassantSquare;
    snapshot.halfmoveClock = halfmoveClock;
    snapshot.fullmoveNumber = fullmoveNumber;
    snapshot.from = from;
    snapshot.to = to;
    snapshot.whiteToMove = whiteToMove;

    snapshot.capturedPiece = capturedPiece;
    snapshot.movedPiece = movedPiece;
    snapshot.promotedPiece = charToPiece(move.promotion);
    snapshot.enPassant = move.isEnPassant;
    snapshot.whiteKingPos = whiteKingPos;
    snapshot.blackKingPos = blackKingPos;

    history[ply++] = snapshot;
    
    ++halfmoveClock;
    enPassantSquare = 0;
    pieceAt[to] = movedPiece;

    Color mySide = isWhite ? WHITE : BLACK;

    if (movedPiece == PAWN) {
        bitboards[mySide][0] &= ~fromBB;
        bitboards[mySide][0] |= toBB;
        halfmoveClock = 0;
        if (move.isEnPassant) {
            int capSq = isWhite ? to - 8 : to + 8;
            bitboards[1-mySide][0] &= ~bitMasks[capSq];
            pieceAt[capSq] = EMPTY;
        }
        if (isWhite && (fromBB & rank2) && (toBB & rank4)) {
            enPassantSquare = (1 << 6) | (from + 8);
        } else if (!isWhite && (fromBB & rank7) && (toBB & rank5)) {
            enPassantSquare = (1 << 6) | (from - 8);
        }
        if (move.promotion != 0) {
            bitboards[mySide][0] &= ~toBB;
            Piece promoted = charToPiece(move.promotion);
            bitboards[mySide][promoted-1] |= toBB;
            pieceAt[to] = promoted;
        }
    } else if (movedPiece == KING) {
        bitboards[mySide][5] &= ~fromBB;
        bitboards[mySide][5] |= toBB;
        if (mySide == WHITE) whiteKingPos = to;
        else blackKingPos = to;

        if (from == e1 || from == e8) {
            castlingRights &= static_cast<uint8_t>(~static_cast<uint8_t>(3 << (2 * (1 - mySide))));
            uint64_t& rooks = bitboards[mySide][3];
            if (to == g1) {
                rooks = (rooks & ~bitMasks[h1]) | bitMasks[f1];
                pieceAt[h1] = EMPTY;
                pieceAt[f1] = ROOK;
            } else if (to == c1) {
                rooks = (rooks & ~bitMasks[a1]) | bitMasks[d1];
                pieceAt[a1] = EMPTY;
                pieceAt[d1] = ROOK;
            } else if (to == g8) {
                rooks = (rooks & ~bitMasks[h8]) | bitMasks[f8];
                pieceAt[h8] = EMPTY;
                pieceAt[f8] = ROOK;
            } else if (to == c8) {
                rooks = (rooks & ~bitMasks[a8]) | bitMasks[d8];
                pieceAt[a8] = EMPTY;
                pieceAt[d8] = ROOK;
            }
        }

    } else {
        bitboards[mySide][movedPiece-1] &= ~fromBB;
        bitboards[mySide][movedPiece-1] |= toBB;
    }

    castlingRights &= ~(castlingClearTable[from] | castlingClearTable[to]);

    if (capturedPiece != EMPTY && !move.isEnPassant) {
        bitboards[1-mySide][capturedPiece-1] &= ~toBB;
        halfmoveClock = 0;
    }

    pieceAt[from] = EMPTY;

    whiteToMove = !whiteToMove;
    if (!whiteToMove) ++fullmoveNumber;

    UpdateOccupancy();
    UpdateAttacks(*this);
}

void Board::unmake_move() {
    if (ply == 0) return;
    const MoveState& state = history[--ply];

    // Restore game state
    castlingRights = state.castlingRights;
    enPassantSquare = state.enPassantSquare;
    halfmoveClock = state.halfmoveClock;
    whiteToMove = state.whiteToMove;
    fullmoveNumber = state.fullmoveNumber;

    whiteKingPos = state.whiteKingPos;
    blackKingPos = state.blackKingPos;

    uint8_t from = state.from;
    uint8_t to = state.to;
    uint64_t fromBB = bitMasks[from];
    uint64_t toBB = bitMasks[to];

    Color side = whiteToMove ? WHITE : BLACK;
    Color opponent = static_cast<Color>(1 - side);

    Piece moved = state.movedPiece;
    Piece captured = state.capturedPiece;
    Piece promoted = state.promotedPiece;

    // Handle pieceAt[] restoration
    pieceAt[from] = moved;
    pieceAt[to] = captured;

    // Handle promotions
    if (moved == PAWN && promoted != EMPTY) {
        // Remove promoted piece from bitboard
        bitboards[side][promoted - 1] &= ~toBB;
        // Add pawn back
        bitboards[side][PAWN - 1] |= fromBB;
    } else {
        // Undo normal piece movement
        bitboards[side][moved - 1] &= ~toBB;
        bitboards[side][moved - 1] |= fromBB;
    }

    // Handle captures
    if (captured != EMPTY) {
        bitboards[opponent][captured - 1] |= toBB;
    }

    // Handle en-passant
    if (moved == PAWN && state.enPassant) {
        int capSq = whiteToMove ? to - 8 : to + 8;
        bitboards[opponent][0] |= bitMasks[capSq];
        pieceAt[capSq] = PAWN;
    }

    // Undo castling rook movement
    if (moved == KING) {
        if (from == e1 && to == g1) {
            // White kingside
            bitboards[WHITE][ROOK - 1] &= ~bitMasks[f1];
            bitboards[WHITE][ROOK - 1] |= bitMasks[h1];
            pieceAt[f1] = EMPTY;
            pieceAt[h1] = ROOK;
        } else if (from == e1 && to == c1) {
            // White queenside
            bitboards[WHITE][ROOK - 1] &= ~bitMasks[d1];
            bitboards[WHITE][ROOK - 1] |= bitMasks[a1];
            pieceAt[d1] = EMPTY;
            pieceAt[a1] = ROOK;
        } else if (from == e8 && to == g8) {
            // Black kingside
            bitboards[BLACK][ROOK - 1] &= ~bitMasks[f8];
            bitboards[BLACK][ROOK - 1] |= bitMasks[h8];
            pieceAt[f8] = EMPTY;
            pieceAt[h8] = ROOK;
        } else if (from == e8 && to == c8) {
            // Black queenside
            bitboards[BLACK][ROOK - 1] &= ~bitMasks[d8];
            bitboards[BLACK][ROOK - 1] |= bitMasks[a8];
            pieceAt[d8] = EMPTY;
            pieceAt[a8] = ROOK;
        }
    }

    UpdateOccupancy();   // recompute generalboards[3]
    UpdateAttacks(*this); // update whiteAttacks/blackAttacks
}

bool Board::is_king_in_check(bool white) {
    return white ? (blackAttacks & bitMasks[whiteKingPos]) != 0
                : (whiteAttacks & bitMasks[blackKingPos]) != 0;
}
void Board::UpdateOccupancy() {
    generalboards[WHITE] = bitboards[WHITE][0] | bitboards[WHITE][1] | bitboards[WHITE][2] | bitboards[WHITE][3] | bitboards[WHITE][4] | bitboards[WHITE][5];
    generalboards[BLACK] = bitboards[BLACK][0] | bitboards[BLACK][1] | bitboards[BLACK][2] | bitboards[BLACK][3] | bitboards[BLACK][4] | bitboards[BLACK][5];

    generalboards[2] = generalboards[WHITE] | generalboards[BLACK];

    whiteKingPos = static_cast<uint8_t>(__builtin_ctzll(bitboards[WHITE][5]));
    blackKingPos = static_cast<uint8_t>(__builtin_ctzll(bitboards[BLACK][5]));
}
bool Board::hasEnPassant() const {
    return (enPassantSquare & (1 << 6)) != 0;
}
uint8_t Board::getEnPassantTarget() const {
    // Returns 0–63 square index if en passant is available; undefined otherwise
    return enPassantSquare & 0b00111111;
}