#include "board.h"

void Board::MakeMove(Move move) {
    uint8_t from = move.from;
    uint8_t to = move.to;
    uint64_t fromBB = bitMasks[from];
    uint64_t toBB = bitMasks[to];
    Piece movedPiece = pieceAt[from];
    Piece capturedPiece = pieceAt[to];
    int forwards = whiteToMove ? 8 : -8;

    Color side = whiteToMove ? WHITE : BLACK;
    Color opponent = whiteToMove ? BLACK : WHITE;

    SaveSnapshot(move);

    enPassantSquare = 0;

    bitboards[side][movedPiece - 1] &= ~fromBB;
    generalboards[side] &= ~fromBB;
    generalboards[2] &= ~fromBB;
    bitboards[side][movedPiece - 1] |= toBB; 
    generalboards[side] |= toBB;
    generalboards[2] |= toBB;
    pieceAt[from] = EMPTY;
    pieceAt[to] = movedPiece;

    ++halfmoveClock;
    castlingRights &= ~(castlingClearTable[from] | castlingClearTable[to]);

    if (movedPiece == PAWN) {
        halfmoveClock = 0;
        bool isDouble = std::abs(to - from) == 16;
        enPassantSquare = static_cast<uint8_t>(((1 << 6) | static_cast<uint8_t>(from + forwards)) * isDouble * (movedPiece == PAWN)); 
    }
    if (capturedPiece != EMPTY) {
        halfmoveClock = 0;
        bitboards[opponent][capturedPiece - 1] &= ~toBB;
        generalboards[opponent] &= ~toBB;
    }
    if (move.promotion != 0) {
        Piece promotion = NeptuneInternals::charToPiece[static_cast<u_char>(move.promotion)];
        bitboards[side][PAWN - 1] &= ~toBB;
        bitboards[side][promotion - 1] |= toBB;
        pieceAt[to] = promotion;
    }
    if (move.isEnPassant) {
        int capSq = to - forwards;
        uint64_t capBB = bitMasks[capSq];
        bitboards[opponent][PAWN - 1] &= ~capBB;
        generalboards[opponent] &= ~capBB;
        generalboards[2] &= ~capBB;
        pieceAt[capSq] = EMPTY;
    }
    if (movedPiece == KING) {
        kings[side] = to;
        castlingRights &= ~(0b1100 >> (2 * side));
        uint64_t rookToFrom = castleXOR[from][to];
        if (rookToFrom != 0) {
            bitboards[side][ROOK - 1] ^= rookToFrom;
            generalboards[side] ^= rookToFrom;
            generalboards[2] ^= rookToFrom;

            uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(rookToFrom));
            rookToFrom &= rookToFrom - 1;
            pieceAt[sq] = static_cast<Piece>(ROOK - pieceAt[sq]);
            sq = static_cast<uint8_t>(__builtin_ctzll(rookToFrom));
            pieceAt[sq] = static_cast<Piece>(ROOK - pieceAt[sq]);
        }
    }

    whiteToMove = !whiteToMove;
    zobristKey ^= zobristSideToMove;
    fullmoveNumber += !whiteToMove;
}

void Board::UnmakeMove() {
    if (ply == 0) return;
    uint64_t state = history[--ply].GetState();

    uint8_t to = state & 0x3F;
    uint8_t from = (state >> 6) & 0x3F;
    Piece movedPiece = static_cast<Piece>((state >> 12) & 0x7);
    Piece capturedPiece = static_cast<Piece>((state >> 15) & 0x7);
    Piece promoted = static_cast<Piece>((state >> 18) & 0x7);
    castlingRights = static_cast<uint8_t>((state >> 21) & 0xF);
    enPassantSquare = static_cast<uint8_t>((state >> 25) & 0x3F);
    halfmoveClock = static_cast<uint8_t>((state >> 32) & 0x3F);
    bool isEnpassant = static_cast<bool>((state >> 39) & 0x1);

    whiteToMove = !whiteToMove;

    Color side = whiteToMove ? WHITE : BLACK;
    Color opponent = whiteToMove ? BLACK : WHITE;

    uint64_t fromBB = bitMasks[from];
    uint64_t toBB = bitMasks[to];

    int forwards = whiteToMove ? 8 : -8;

    bitboards[side][movedPiece - 1] &= ~toBB;
    bitboards[side][movedPiece - 1] |= fromBB;
    generalboards[side] &= ~toBB;
    generalboards[side] |= fromBB;
    generalboards[2] &= ~toBB;
    generalboards[2] |= fromBB;
    pieceAt[to] = EMPTY;
    pieceAt[from] = movedPiece;

    if (capturedPiece != EMPTY) {
        bitboards[opponent][capturedPiece - 1] |= toBB;
        generalboards[opponent] |= toBB;
        generalboards[2] |= toBB;
        pieceAt[to] = capturedPiece;
    }
    if (promoted != EMPTY) {
        bitboards[side][promoted - 1] &= ~toBB;
    }
    if (isEnpassant) {
        int capSq = to - forwards;
        uint64_t capBB = bitMasks[capSq];
        bitboards[opponent][PAWN - 1] |= capBB;
        generalboards[opponent] |= capBB;
        generalboards[2] |= capBB;
        pieceAt[capSq] = PAWN;
    }
    if (movedPiece == KING) {
        kings[side] = from;
        uint64_t rookToFrom = castleXOR[from][to];
        if (rookToFrom != 0) {
            bitboards[side][ROOK - 1] ^= rookToFrom;
            generalboards[side] ^= rookToFrom;
            generalboards[2] ^= rookToFrom;

            uint8_t sq = static_cast<uint8_t>(__builtin_ctzll(rookToFrom));
            rookToFrom &= rookToFrom - 1;
            pieceAt[sq] = static_cast<Piece>(ROOK - pieceAt[sq]);
            sq = static_cast<uint8_t>(__builtin_ctzll(rookToFrom));
            pieceAt[sq] = static_cast<Piece>(ROOK - pieceAt[sq]);
        }
    }
    
    fullmoveNumber -= (side == WHITE);
}

bool Board::hasEnPassant() const {
    return (enPassantSquare & (1 << 6)) != 0;
}
bool Board::isKingInCheck(bool white) {
    return white ? (isSquareAttacked(kings[WHITE], BLACK, false))
                : (isSquareAttacked(kings[BLACK], WHITE, false));
}
bool Board::isSquareAttacked(uint8_t square, Color attacker, bool excludeKing) {
    if (attacksBB[KNIGHT-1][attacker][square] & bitboards[attacker][KNIGHT-1]) return true;
    if (attacksBB[KING-1][attacker][square] & bitboards[attacker][KING-1]) return true;
    if (attacksBB[PAWN-1][1-attacker][square] & bitboards[attacker][PAWN-1]) return true;

    uint64_t all = generalboards[2];
    if (excludeKing) {
        all = generalboards[2] ^ bitboards[1-attacker][KING-1];
    }

    uint64_t rookLike = ROOK_ATTACKS(square, all);
    if (rookLike & (bitboards[attacker][ROOK-1] | bitboards[attacker][QUEEN-1])) return true;

    uint64_t bishopLike = BISHOP_ATTACKS(square, all);
    if (bishopLike & (bitboards[attacker][BISHOP-1] | bitboards[attacker][QUEEN-1])) return true;

    return false;
}

bool Board::isSquareAttacked(uint8_t* squares, int numSquares, Color attacker, bool excludeKing) {
    for (int i = 0; i < numSquares; ++i) {
        if (isSquareAttacked(squares[i], attacker, excludeKing)) return true;
    }
    return false;
}
uint8_t Board::getEnPassantTarget() const {
    // Returns 0–63 square index if en passant is available; undefined otherwise
    return enPassantSquare & 0b00111111;
}

uint64_t Board::GetAttackersTo(uint8_t square, Color attackingSide) {
    uint64_t occupancy = generalboards[2];
    uint64_t attackers = 0ULL;

    // Pawns
    attackers |= attacksBB[PAWN-1][1-attackingSide][square] & bitboards[attackingSide][PAWN-1];

    // Knights
    attackers |= attacksBB[KNIGHT-1][attackingSide][square] & bitboards[attackingSide][KNIGHT - 1];

    // Kings (for completeness, though rarely needed)
    attackers |= attacksBB[KING-1][attackingSide][square] & bitboards[attackingSide][KING - 1];

    // Bishops / Queens
    uint64_t bishopSliders = bitboards[attackingSide][BISHOP - 1] | bitboards[attackingSide][QUEEN - 1];
    attackers |= BISHOP_ATTACKS(square, occupancy) & bishopSliders;

    // Rooks / Queens
    uint64_t rookSliders = bitboards[attackingSide][ROOK - 1] | bitboards[attackingSide][QUEEN - 1];
    attackers |= ROOK_ATTACKS(square, occupancy) & rookSliders;

    return attackers;
}

void Board::UpdatePins(Color forSide) {
    globalPinVersion += 1;
    uint64_t diagonals = bitboards[1-forSide][BISHOP-1] | bitboards[1-forSide][QUEEN-1];
    uint64_t orthogonals = bitboards[1-forSide][ROOK-1] | bitboards[1-forSide][QUEEN-1];

    uint64_t diagonalAttackers = BISHOP_ATTACKS(kings[forSide], generalboards[1-forSide]) & diagonals;
    uint64_t orthogonalAttackers = ROOK_ATTACKS(kings[forSide], generalboards[1-forSide]) & orthogonals;

    while (diagonalAttackers) {
        int sq = __builtin_ctzll(diagonalAttackers);
        diagonalAttackers &= diagonalAttackers - 1;
        uint64_t pinRay = lineBB[sq][kings[forSide]];
        uint64_t piecesOnRay = pinRay & generalboards[forSide];
        //Either there is no piece to be pinned or there are more blockers.
        if (__builtin_popcountll(piecesOnRay) != 1) continue;

        uint8_t pinnedSquare = static_cast<uint8_t>(__builtin_ctzll(piecesOnRay));
        pinVersion[pinnedSquare] = globalPinVersion;
        pinMask[pinnedSquare] = pinRay | bitMasks[sq];
    }

    while (orthogonalAttackers) {
        int sq = __builtin_ctzll(orthogonalAttackers);
        orthogonalAttackers &= orthogonalAttackers - 1;
        uint64_t pinRay = lineBB[sq][kings[forSide]];
        uint64_t piecesOnRay = pinRay & generalboards[forSide];
        //Either there is no piece to be pinned or there are more blockers.
        if (__builtin_popcountll(piecesOnRay) != 1) continue;

        uint8_t pinnedSquare = static_cast<uint8_t>(__builtin_ctzll(piecesOnRay));
        pinVersion[pinnedSquare] = globalPinVersion;
        pinMask[pinnedSquare] = pinRay | bitMasks[sq];
    }
}

void Board::SaveSnapshot(Move move) {
    MoveState& snapshot = history[ply++];
    uint64_t state = 0;

    state |= static_cast<uint64_t>(move.isEnPassant & 0x1) << 39;
    state |= static_cast<uint64_t>(halfmoveClock & 0x7F) << 32;
    state |= static_cast<uint64_t>(enPassantSquare & 0x7F) << 25;
    state |= static_cast<uint64_t>(castlingRights & 0xF) << 21;
    state |= static_cast<uint64_t>(charToPiece[move.promotion] & 0x7) << 18;
    state |= static_cast<uint64_t>(pieceAt[move.to] & 0x7) << 15;
    state |= static_cast<uint64_t>(pieceAt[move.from] & 0x7) << 12;
    state |= static_cast<uint64_t>(move.from & 0x3F) << 6;
    state |= static_cast<uint64_t>(move.to & 0x3F);

    snapshot.SetState(state);
}

void Board::UpdateOccupancy() { 
    generalboards[WHITE] = bitboards[WHITE][0] | bitboards[WHITE][1] | bitboards[WHITE][2] | bitboards[WHITE][3] | bitboards[WHITE][4] | bitboards[WHITE][5]; 
    generalboards[BLACK] = bitboards[BLACK][0] | bitboards[BLACK][1] | bitboards[BLACK][2] | bitboards[BLACK][3] | bitboards[BLACK][4] | bitboards[BLACK][5]; 
    generalboards[2] = generalboards[WHITE] | generalboards[BLACK]; 
    kings[WHITE] = static_cast<uint8_t>(__builtin_ctzll(bitboards[WHITE][5])); 
    kings[BLACK] = static_cast<uint8_t>(__builtin_ctzll(bitboards[BLACK][5])); 
}