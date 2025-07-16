#include "board.h"

void Board::MakeMove(Move move) {
    uint8_t from = move.from;
    uint8_t to = move.to;
    bool isWhite = whiteToMove;
    uint64_t fromBB = bitMasks[from];
    uint64_t toBB = bitMasks[to];
    Piece movedPiece = pieceAt[from];
    Piece capturedPiece = pieceAt[to];
    int forwards = isWhite ? 8 : -8;
    
    MoveState& snapshot = history[ply++];
    snapshot.castlingRights = castlingRights;
    snapshot.enPassantSquare = enPassantSquare;
    snapshot.halfmoveClock = halfmoveClock;
    snapshot.fullmoveNumber = fullmoveNumber;
    snapshot.from = from;
    snapshot.to = to;
    snapshot.whiteToMove = whiteToMove;

    snapshot.capturedPiece = capturedPiece;
    snapshot.movedPiece = movedPiece;
    snapshot.promotedPiece = charToPiece[static_cast<u_char>(move.promotion)];
    snapshot.enPassant = move.isEnPassant;
    snapshot.whiteKingPos = kings[WHITE];
    snapshot.blackKingPos = kings[BLACK];
    snapshot.zobristKey = zobristKey;
    
    ++halfmoveClock;

    if (enPassantSquare != 0)
        zobristKey ^= zobristEnPassant[enPassantSquare & 7];

    enPassantSquare = 0;
    pieceAt[to] = movedPiece;

    Color mySide = isWhite ? WHITE : BLACK;

    if (movedPiece == EMPTY) {
        std::cout << "Moving nothing\n" << std::flush;
    }

    zobristKey ^= zobristPiece[mySide][movedPiece - 1][from];
    zobristKey ^= zobristPiece[mySide][movedPiece - 1][to];

    bitboards[mySide][movedPiece-1] &= ~fromBB;
    bitboards[mySide][movedPiece-1] |= toBB;

    if (movedPiece == KING) {
        kings[mySide] = to;

        uint64_t rookToFrom = castleXOR[from][to];
        uint64_t& rooks = bitboards[mySide][3];

        castlingRights &= static_cast<uint8_t>(~static_cast<uint8_t>(3 << (2 * (1 - mySide)))); // Clear castling rights
        rooks ^= rookToFrom;

        if (rookToFrom) {
            //First bit of rookToFrom
            uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(rookToFrom));
            rookToFrom &= rookToFrom - 1;

            pieceAt[sq] = static_cast<Piece>(ROOK - pieceAt[sq]);

            //Second bit of rookToFrom
            sq = static_cast<uint8_t>(__builtin_ctzll(rookToFrom));

            pieceAt[sq] = static_cast<Piece>(ROOK - pieceAt[sq]);
        }
    }

    zobristKey ^= zobristCastling[castlingRights];
    castlingRights &= ~(castlingClearTable[from] | castlingClearTable[to]);
    zobristKey ^= zobristCastling[castlingRights];

    // If its not a double push, this will evaluate to 0
    bool isDouble = std::abs(to - from) == 16;
    enPassantSquare = static_cast<uint8_t>(((1 << 6) | static_cast<uint8_t>(from + forwards)) * isDouble * (movedPiece == PAWN)); 

    if (enPassantSquare != 0)
        zobristKey ^= zobristEnPassant[enPassantSquare & 7];

    if (__builtin_expect(move.isEnPassant, 0)) {
        int capSq = to - forwards;
        zobristKey ^= zobristPiece[1 - mySide][PAWN - 1][capSq];
        bitboards[1-mySide][0] &= ~bitMasks[capSq];
        pieceAt[capSq] = EMPTY;
    } else if (__builtin_expect(move.promotion != 0, 0)) {
        bitboards[mySide][0] &= ~toBB;
        Piece promoted = NeptuneInternals::charToPiece[static_cast<u_char>(move.promotion)];
        zobristKey ^= zobristPiece[mySide][movedPiece - 1][to];       // Undo pawn placement
        zobristKey ^= zobristPiece[mySide][promoted - 1][to];         // Add promoted piece
        bitboards[mySide][promoted-1] |= toBB;
        pieceAt[to] = promoted;
    }

    if (capturedPiece != EMPTY && !move.isEnPassant) {
        zobristKey ^= zobristPiece[1 - mySide][capturedPiece - 1][to];
        bitboards[1-mySide][capturedPiece-1] &= ~toBB;
        halfmoveClock = 0;
    }

    pieceAt[from] = EMPTY;

    whiteToMove = !whiteToMove;
    zobristKey ^= zobristSideToMove;
    fullmoveNumber += !whiteToMove;

    UpdateOccupancy();
}

void Board::UnmakeMove() {
    if (ply == 0) return;
    const MoveState& state = history[--ply];

    // Restore game state
    castlingRights = state.castlingRights;
    enPassantSquare = state.enPassantSquare;
    halfmoveClock = state.halfmoveClock;
    whiteToMove = state.whiteToMove;
    fullmoveNumber = state.fullmoveNumber;

    kings[WHITE] = state.whiteKingPos;
    kings[BLACK] = state.blackKingPos;

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

    zobristKey = state.zobristKey;

    UpdateOccupancy();   // recompute generalboards[3]
}
void Board::UpdateOccupancy() {
    generalboards[WHITE] = bitboards[WHITE][0] | bitboards[WHITE][1] | bitboards[WHITE][2] | bitboards[WHITE][3] | bitboards[WHITE][4] | bitboards[WHITE][5];
    generalboards[BLACK] = bitboards[BLACK][0] | bitboards[BLACK][1] | bitboards[BLACK][2] | bitboards[BLACK][3] | bitboards[BLACK][4] | bitboards[BLACK][5];

    generalboards[2] = generalboards[WHITE] | generalboards[BLACK];

    kings[WHITE] = static_cast<uint8_t>(__builtin_ctzll(bitboards[WHITE][5]));
    kings[BLACK] = static_cast<uint8_t>(__builtin_ctzll(bitboards[BLACK][5]));
}
bool Board::hasEnPassant() const {
    return (enPassantSquare & (1 << 6)) != 0;
}
bool Board::isKingInCheck(bool white) {
    return white ? (blackAttacks & bitMasks[kings[WHITE]]) != 0
                : (whiteAttacks & bitMasks[kings[BLACK]]) != 0;
}
bool Board::isSquareAttacked(uint8_t square, Color attacker) {
    if (attacksBB[KNIGHT-1][attacker][square] & bitboards[attacker][KNIGHT-1]) return true;
    if (attacksBB[KING-1][attacker][square] & bitboards[attacker][KING-1]) return true;
    if (attacksBB[PAWN-1][1-attacker][square] & bitboards[attacker][PAWN-1]) return true;

    const uint64_t all = generalboards[2];

    uint64_t rookLike = ROOK_ATTACKS(square, all);
    if (rookLike & (bitboards[attacker][ROOK-1] | bitboards[attacker][QUEEN-1])) return true;

    uint64_t bishopLike = BISHOP_ATTACKS(square, all);
    if (bishopLike & (bitboards[attacker][BISHOP-1] | bitboards[attacker][QUEEN-1])) return true;

    return false;
}
uint8_t Board::getEnPassantTarget() const {
    // Returns 0–63 square index if en passant is available; undefined otherwise
    return enPassantSquare & 0b00111111;
}