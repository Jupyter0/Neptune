#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <cstdint>
#include <sstream>
#include <cctype>
#include <iterator>
#include <random>
#include <chrono>
#include <cstring>


using namespace std;

constexpr int bishopDirs[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
constexpr int rookDirs[4][2] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
constexpr int queenDirs[8][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}, {0, 1}, {0, -1}, {-1, 0}, {1, 0}};
constexpr int knightDirs[8][2] = {{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}};
constexpr int kingDirs[8][2] = {{1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}};

constexpr std::array<std::array<uint64_t, 64>, 2> pawnAttacks = {{
    // WHITE
    {
        0x0000000000000200ULL, 0x0000000000000500ULL, 0x0000000000000A00ULL, 0x0000000000001400ULL, 
        0x0000000000002800ULL, 0x0000000000005000ULL, 0x000000000000A000ULL, 0x0000000000004000ULL,
        0x0000000000020000ULL, 0x0000000000050000ULL, 0x00000000000A0000ULL, 0x0000000000140000ULL, 
        0x0000000000280000ULL, 0x0000000000500000ULL, 0x0000000000A00000ULL, 0x0000000000400000ULL, 
        0x0000000002000000ULL, 0x0000000005000000ULL, 0x000000000A000000ULL, 0x0000000014000000ULL, 
        0x0000000028000000ULL, 0x0000000050000000ULL, 0x00000000A0000000ULL, 0x0000000040000000ULL, 
        0x0000000200000000ULL, 0x0000000500000000ULL, 0x0000000A00000000ULL, 0x0000001400000000ULL, 
        0x0000002800000000ULL, 0x0000005000000000ULL, 0x000000A000000000ULL, 0x0000004000000000ULL, 
        0x0000020000000000ULL, 0x0000050000000000ULL, 0x00000A0000000000ULL, 0x0000140000000000ULL, 
        0x0000280000000000ULL, 0x0000500000000000ULL, 0x0000A00000000000ULL, 0x0000400000000000ULL, 
        0x0002000000000000ULL, 0x0005000000000000ULL, 0x000A000000000000ULL, 0x0014000000000000ULL, 
        0x0028000000000000ULL, 0x0050000000000000ULL, 0x00A0000000000000ULL, 0x0040000000000000ULL, 
        0x0200000000000000ULL, 0x0500000000000000ULL, 0x0A00000000000000ULL, 0x1400000000000000ULL, 
        0x2800000000000000ULL, 0x5000000000000000ULL, 0xA000000000000000ULL, 0x4000000000000000ULL, 
        0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 
        0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL  
    },

    // BLACK
    {
        0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 
        0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
        0x0000000000000002ULL, 0x0000000000000005ULL, 0x000000000000000AULL, 0x0000000000000014ULL, 
        0x0000000000000028ULL, 0x0000000000000050ULL, 0x00000000000000A0ULL, 0x0000000000000040ULL,
        0x0000000000000200ULL, 0x0000000000000500ULL, 0x0000000000000A00ULL, 0x0000000000001400ULL, 
        0x0000000000002800ULL, 0x0000000000005000ULL, 0x000000000000A000ULL, 0x0000000000004000ULL, 
        0x0000000000020000ULL, 0x0000000000050000ULL, 0x00000000000A0000ULL, 0x0000000000140000ULL, 
        0x0000000000280000ULL, 0x0000000000500000ULL, 0x0000000000A00000ULL, 0x0000000000400000ULL, 
        0x0000000002000000ULL, 0x0000000005000000ULL, 0x000000000A000000ULL, 0x0000000014000000ULL, 
        0x0000000028000000ULL, 0x0000000050000000ULL, 0x00000000A0000000ULL, 0x0000000040000000ULL, 
        0x0000000200000000ULL, 0x0000000500000000ULL, 0x0000000A00000000ULL, 0x0000001400000000ULL, 
        0x0000002800000000ULL, 0x0000005000000000ULL, 0x000000A000000000ULL, 0x0000004000000000ULL, 
        0x0000020000000000ULL, 0x0000050000000000ULL, 0x00000A0000000000ULL, 0x0000140000000000ULL, 
        0x0000280000000000ULL, 0x0000500000000000ULL, 0x0000A00000000000ULL, 0x0000400000000000ULL, 
        0x0002000000000000ULL, 0x0005000000000000ULL, 0x000A000000000000ULL, 0x0014000000000000ULL, 
        0x0028000000000000ULL, 0x0050000000000000ULL, 0x00A0000000000000ULL, 0x0040000000000000ULL
    }
}};

constexpr uint8_t castlingClearTable[64] = {
    0b0100, 0, 0, 0, 0, 0, 0, 0b1000,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0b0001, 0, 0, 0, 0, 0, 0, 0b0010
};

constexpr uint64_t knightAttacks[64] = {
    0x0000000000020400ULL, 0x0000000000050800ULL, 0x00000000000a1100ULL, 0x0000000000142200ULL,
    0x0000000000284400ULL, 0x0000000000508800ULL, 0x0000000000a01000ULL, 0x0000000000402000ULL,
    0x0000000002040004ULL, 0x0000000005080008ULL, 0x000000000a110011ULL, 0x0000000014220022ULL,
    0x0000000028440044ULL, 0x0000000050880088ULL, 0x00000000a0100010ULL, 0x0000000040200020ULL,
    0x0000000204000402ULL, 0x0000000508000805ULL, 0x0000000a1100110aULL, 0x0000001422002214ULL,
    0x0000002844004428ULL, 0x0000005088008850ULL, 0x000000a0100010a0ULL, 0x0000004020002040ULL,
    0x0000020400040200ULL, 0x0000050800080500ULL, 0x00000a1100110a00ULL, 0x0000142200221400ULL,
    0x0000284400442800ULL, 0x0000508800885000ULL, 0x0000a0100010a000ULL, 0x0000402000204000ULL,
    0x0002040004020000ULL, 0x0005080008050000ULL, 0x000a1100110a0000ULL, 0x0014220022140000ULL,
    0x0028440044280000ULL, 0x0050880088500000ULL, 0x00a0100010a00000ULL, 0x0040200020400000ULL,
    0x0204000402000000ULL, 0x0508000805000000ULL, 0x0a1100110a000000ULL, 0x1422002214000000ULL,
    0x2844004428000000ULL, 0x5088008850000000ULL, 0xa0100010a0000000ULL, 0x4020002040000000ULL,
    0x0400040200000000ULL, 0x0800080500000000ULL, 0x1100110a00000000ULL, 0x2200221400000000ULL,
    0x4400442800000000ULL, 0x8800885000000000ULL, 0x100010a000000000ULL, 0x2000204000000000ULL,
    0x0004020000000000ULL, 0x0008050000000000ULL, 0x00110a0000000000ULL, 0x0022140000000000ULL,
    0x0044280000000000ULL, 0x0088500000000000ULL, 0x0010a00000000000ULL, 0x0020400000000000ULL
};
constexpr uint64_t kingAttacks[64] = {
    0x0000000000000302ULL, 0x0000000000000705ULL, 0x0000000000000e0aULL, 0x0000000000001c14ULL,
    0x0000000000003828ULL, 0x0000000000007050ULL, 0x000000000000e0a0ULL, 0x000000000000c040ULL,
    0x0000000000030203ULL, 0x0000000000070507ULL, 0x00000000000e0a0eULL, 0x00000000001c141cULL,
    0x0000000000382838ULL, 0x0000000000705070ULL, 0x0000000000e0a0e0ULL, 0x0000000000c040c0ULL,
    0x0000000003020300ULL, 0x0000000007050700ULL, 0x000000000e0a0e00ULL, 0x000000001c141c00ULL,
    0x0000000038283800ULL, 0x0000000070507000ULL, 0x00000000e0a0e000ULL, 0x00000000c040c000ULL,
    0x0000000302030000ULL, 0x0000000705070000ULL, 0x0000000e0a0e0000ULL, 0x0000001c141c0000ULL,
    0x0000003828380000ULL, 0x0000007050700000ULL, 0x000000e0a0e00000ULL, 0x000000c040c00000ULL,
    0x0000030203000000ULL, 0x0000070507000000ULL, 0x00000e0a0e000000ULL, 0x00001c141c000000ULL,
    0x0000382838000000ULL, 0x0000705070000000ULL, 0x0000e0a0e0000000ULL, 0x0000c040c0000000ULL,
    0x0003020300000000ULL, 0x0007050700000000ULL, 0x000e0a0e00000000ULL, 0x001c141c00000000ULL,
    0x0038283800000000ULL, 0x0070507000000000ULL, 0x00e0a0e000000000ULL, 0x00c040c000000000ULL,
    0x0302030000000000ULL, 0x0705070000000000ULL, 0x0e0a0e0000000000ULL, 0x1c141c0000000000ULL,
    0x3828380000000000ULL, 0x7050700000000000ULL, 0xe0a0e00000000000ULL, 0xc040c00000000000ULL,
    0x0203000000000000ULL, 0x0507000000000000ULL, 0x0a0e000000000000ULL, 0x141c000000000000ULL,
    0x2838000000000000ULL, 0x5070000000000000ULL, 0xa0e0000000000000ULL, 0x40c0000000000000ULL
};

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

constexpr uint64_t bitMasks[64] = {
    1ULL << a1, 1ULL << b1, 1ULL << c1, 1ULL << d1, 1ULL << e1, 1ULL << f1, 1ULL << g1, 1ULL << h1, 
    1ULL << a2, 1ULL << b2, 1ULL << c2, 1ULL << d2, 1ULL << e2, 1ULL << f2, 1ULL << g2, 1ULL << h2, 
    1ULL << a3, 1ULL << b3, 1ULL << c3, 1ULL << d3, 1ULL << e3, 1ULL << f3, 1ULL << g3, 1ULL << h3, 
    1ULL << a4, 1ULL << b4, 1ULL << c4, 1ULL << d4, 1ULL << e4, 1ULL << f4, 1ULL << g4, 1ULL << h4, 
    1ULL << a5, 1ULL << b5, 1ULL << c5, 1ULL << d5, 1ULL << e5, 1ULL << f5, 1ULL << g5, 1ULL << h5, 
    1ULL << a6, 1ULL << b6, 1ULL << c6, 1ULL << d6, 1ULL << e6, 1ULL << f6, 1ULL << g6, 1ULL << h6, 
    1ULL << a7, 1ULL << b7, 1ULL << c7, 1ULL << d7, 1ULL << e7, 1ULL << f7, 1ULL << g7, 1ULL << h7, 
    1ULL << a8, 1ULL << b8, 1ULL << c8, 1ULL << d8, 1ULL << e8, 1ULL << f8, 1ULL << g8, 1ULL << h8, 
};

constexpr uint64_t castlingBB[4] = {
    (1ULL << f1) | (1ULL << g1),              // White king-side
    (1ULL << b1) | (1ULL << c1) | (1ULL << d1), // White queen-side
    (1ULL << f8) | (1ULL << g8),              // Black king-side
    (1ULL << b8) | (1ULL << c8) | (1ULL << d8)  // Black queen-side
};

constexpr uint64_t rank1 = 0x00000000000000FFULL;
constexpr uint64_t rank2 = 0x000000000000FF00ULL;
constexpr uint64_t rank3 = 0x0000000000FF0000ULL;
constexpr uint64_t rank4 = 0x00000000FF000000ULL;
constexpr uint64_t rank5 = 0x000000FF00000000ULL;
constexpr uint64_t rank6 = 0x0000FF0000000000ULL;
constexpr uint64_t rank7 = 0x00FF000000000000ULL;
constexpr uint64_t rank8 = 0xFF00000000000000ULL;



enum Piece {
    EMPTY, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};

enum Color {
    WHITE, BLACK
};

struct Move {
    int from;
    int to;
    char promotion = 0;
    bool isEnPassant = false;

    Move(int From, int To, char Promotion = 0, bool EP = false) : from(From), to(To), promotion(Promotion), isEnPassant(EP) {}
};

constexpr array<Piece, 128> promotionCharToPiece = [] {
    array<Piece, 128> table{};
    table['q'] = QUEEN;
    table['r'] = ROOK;
    table['b'] = BISHOP;
    table['n'] = KNIGHT;
    return table;
}();

string indexToSquare(int index) {
    int file = index & 7;  // 0 to 7
    int rank = index >> 3;  // 0 to 7

    char fileChar = 'a' + file;       // 'a' to 'h'
    char rankChar = '1' + rank;       // '1' to '8'

    return string() + fileChar + rankChar;
}

Piece charToPiece(char pieceChar) {
    pieceChar = tolower(pieceChar);
    switch (pieceChar) {
        case 'p': return PAWN;
        case 'n': return KNIGHT;
        case 'b': return BISHOP;
        case 'r': return ROOK;
        case 'q': return QUEEN;
        case 'k': return KING;
        default: return EMPTY;
    }
}

class Board {
public:
    uint64_t whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing;
    uint64_t blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing;

    uint64_t whitePieces;
    uint64_t blackPieces;
    uint64_t allPieces;

    uint8_t whiteKingPos;
    uint8_t blackKingPos;

    uint64_t whiteAttacks;
    uint64_t blackAttacks;

    bool whiteToMove;

    uint8_t castlingRights; //0b0000KQkq K: WKS, Q: WQS, k: BKS, q: BQS
    uint8_t enPassantSquare; //0b0fdddddd f: flag, d: data

    uint8_t halfmoveClock; //Used for 50-move rule
    uint8_t fullmoveNumber; //Counts the move number

    uint64_t zobristKey = 0ULL; //Zobrist Hash !!Don't use this, this feature is not yet implemented!!

    Piece pieceAt[64];

    void setBB(const string& fen) {
        if (fen.length() == 0) return;
        istringstream fss(fen);
        string field;
        vector<string> fields;

        while (fss >> field) {
            fields.push_back(field);
        }

        if (fields.size() < 6) {
            cerr << "Invalid FEN: not enough fields\n";
            return;
        }

        ParsePieces(fields[0]);

        whiteToMove = (fields[1] == "w");

        castlingRights = 0ULL;
        if (fields[2] != "-") {
            for (char c : fields[2]) {
                switch (c) {
                    case 'K': castlingRights |= 1 << 3; break;
                    case 'Q': castlingRights |= 1 << 2; break;
                    case 'k': castlingRights |= 1 << 1; break;
                    case 'q': castlingRights |= 1 << 0; break;
                }
            }
        }

        constexpr uint8_t epFlag = 1 << 6;
        enPassantSquare = 0ULL;
        if (fields[3] != "-") {
            int file = fields[3][0] - 'a';
            int rank = fields[3][1] - '1';
            if (file >= 0 && file < 8 && rank >= 0 && rank < 8) {
                enPassantSquare = epFlag | (rank * 8 + file);
            } else {
                cerr << "[Warning] Invalid en passant square in FEN: " << fields[3] << endl;
            }
        }

        halfmoveClock = std::stoi(fields[4]);
        fullmoveNumber = std::stoi(fields[5]);

        UpdateOccupancy();
        UpdateAttacks();
    }

    void UpdateAttacks() {
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
    void AddSlidingAttacks(uint64_t pieces, Piece pieceType, Color color, uint64_t& attacks) {
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
            }
        }
    }

    uint64_t CalculateSlidingAttacks(uint64_t sq, const int directions[][2], uint64_t all) {
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

    void AddNonSlidingAttacks(uint64_t pieces, Piece pieceType, Color color, uint64_t& attacks) {
        if (pieceType == BISHOP || pieceType == ROOK || pieceType == QUEEN) return;
        uint64_t p = pieces;
        while(p) {
            int sq = __builtin_ctzll(p);
            p &= p - 1;

            switch (pieceType) {
                case KNIGHT: attacks |= knightAttacks[sq]; break;
                case KING: attacks |= kingAttacks[sq]; break;
                case PAWN: attacks |= pawnAttacks[color][sq]; break;
            }
        }
    }

    void ParsePieces(string piecesField) {
        vector<string> ranks;
        size_t start = 0;
        stringstream ss(piecesField);
        string rank;
        while (getline(ss, rank, '/')) ranks.push_back(rank);

        whitePawns = whiteKnights = whiteBishops = whiteRooks = whiteQueens = whiteKing = 0ULL;
        blackPawns = blackKnights = blackBishops = blackRooks = blackQueens = blackKing = 0ULL;

        for (uint8_t i = 0; i < 8 ; i++) {
            uint8_t file = 0;
            for (uint8_t j = 0; j < ranks[i].length(); j++) {
                char piece = ranks[i][j];
                if (isdigit(piece)) {
                    for (int k = 0; k < (piece - '0'); k++) {
                        pieceAt[(7-i)*8 + file] = EMPTY;
                        file++;
                    }
                    continue;
                }
                uint8_t squareIndex = (7-i)*8 + file; // Board square index: a1 = 0, h8 = 63 (bottom-left to top-right)
                switch (piece) {
                        case 'P': whitePawns     |= bitMasks[squareIndex]; pieceAt[squareIndex] = PAWN;   break;
                        case 'N': whiteKnights   |= bitMasks[squareIndex]; pieceAt[squareIndex] = KNIGHT; break;
                        case 'B': whiteBishops   |= bitMasks[squareIndex]; pieceAt[squareIndex] = BISHOP; break;
                        case 'R': whiteRooks     |= bitMasks[squareIndex]; pieceAt[squareIndex] = ROOK;   break;
                        case 'Q': whiteQueens    |= bitMasks[squareIndex]; pieceAt[squareIndex] = QUEEN;  break;
                        case 'K': whiteKing      |= bitMasks[squareIndex]; pieceAt[squareIndex] = KING;   break;

                        case 'p': blackPawns     |= bitMasks[squareIndex]; pieceAt[squareIndex] = PAWN;   break;
                        case 'n': blackKnights   |= bitMasks[squareIndex]; pieceAt[squareIndex] = KNIGHT; break;
                        case 'b': blackBishops   |= bitMasks[squareIndex]; pieceAt[squareIndex] = BISHOP; break;
                        case 'r': blackRooks     |= bitMasks[squareIndex]; pieceAt[squareIndex] = ROOK;   break;
                        case 'q': blackQueens    |= bitMasks[squareIndex]; pieceAt[squareIndex] = QUEEN;  break;
                        case 'k': blackKing      |= bitMasks[squareIndex]; pieceAt[squareIndex] = KING;   break;
                        default: cerr << "[WARNING]: Unknown piece character '" << piece << "'\n"; break;
                }
                file++;
            }
        }
    }

    void make_move(Move move) {
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



    bool is_king_in_check(bool white) {
        return white ? (blackAttacks & bitMasks[whiteKingPos]) != 0
                    : (whiteAttacks & bitMasks[blackKingPos]) != 0;
    }



    void UpdateOccupancy() {
        whitePieces = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing;
        blackPieces = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing;
        allPieces = whitePieces | blackPieces;

        whiteKingPos = __builtin_ctzll(whiteKing);
        blackKingPos = __builtin_ctzll(blackKing);
    }

    bool hasEnPassant() const {
        return (enPassantSquare & (1 << 6)) != 0;
    }

    uint8_t getEnPassantTarget() const {
        // Returns 0–63 square index if en passant is available; undefined otherwise
        return enPassantSquare & 0b00111111;
    }

};

void GenerateCastlingMoves(uint64_t king, uint64_t rooks, uint8_t castlingRights, uint64_t all, vector<Move>& moves, Color color, uint64_t whiteAttacks, uint64_t blackAttacks) {
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

void GenerateSlidingMoves(uint64_t pieces, const int directions[][2], int dirCount, uint64_t own, uint64_t enemy, vector<Move>& moves) {
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

void GenerateNonSlidingMoves(uint64_t pieces, const uint64_t attackTable[64], uint64_t own, vector<Move>& moves) {
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


void GeneratePawnMoves(uint64_t pawns, uint64_t enemy, uint64_t all, vector<Move>& moves, Color color, uint8_t epSquare = 64) {
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

vector<Move> GeneratePseudoLegalMoves(Board& board, bool isWhiteToMove) {
    vector<Move> moves;
    moves.reserve(256);
    uint8_t epTarget = board.hasEnPassant() ? board.getEnPassantTarget() : 64;

    if (isWhiteToMove) {
        GenerateSlidingMoves(board.whiteBishops, bishopDirs, std::size(bishopDirs), board.whitePieces, board.blackPieces, moves);
        GenerateSlidingMoves(board.whiteRooks,   rookDirs,   std::size(rookDirs),   board.whitePieces, board.blackPieces, moves);
        GenerateSlidingMoves(board.whiteQueens,  queenDirs,  std::size(queenDirs),  board.whitePieces, board.blackPieces, moves);
        GenerateNonSlidingMoves(board.whiteKnights, knightAttacks, board.whitePieces, moves);
        GenerateNonSlidingMoves(board.whiteKing, kingAttacks, board.whitePieces, moves);
        GeneratePawnMoves(board.whitePawns, board.blackPieces, board.allPieces, moves, WHITE, epTarget);
        if (!board.is_king_in_check(true)) GenerateCastlingMoves(board.whiteKing, board.whiteRooks, board.castlingRights, board.allPieces, moves, WHITE, board.whiteAttacks, board.blackAttacks);
    } else {
        GenerateSlidingMoves(board.blackBishops, bishopDirs, std::size(bishopDirs), board.blackPieces, board.whitePieces, moves);
        GenerateSlidingMoves(board.blackRooks,   rookDirs,   std::size(rookDirs),   board.blackPieces, board.whitePieces, moves);
        GenerateSlidingMoves(board.blackQueens,  queenDirs,  std::size(queenDirs),  board.blackPieces, board.whitePieces, moves);
        GenerateNonSlidingMoves(board.blackKnights, knightAttacks, board.blackPieces, moves);
        GenerateNonSlidingMoves(board.blackKing, kingAttacks, board.blackPieces, moves);
        GeneratePawnMoves(board.blackPawns, board.whitePieces, board.allPieces, moves, BLACK, epTarget);
        if (!board.is_king_in_check(false)) GenerateCastlingMoves(board.blackKing, board.blackRooks, board.castlingRights, board.allPieces, moves, BLACK, board.whiteAttacks, board.blackAttacks);
    }

    return moves;
};

vector<Move> GenerateLegalMoves(Board& board) {
    vector<Move> pseudoMoves = GeneratePseudoLegalMoves(board, board.whiteToMove);
    vector<Move> legalMoves;
    legalMoves.reserve(pseudoMoves.size());

    for (const Move& move : pseudoMoves) {
        Board temp = board;
        //cout << "[TIME] ---- " << indexToSquare(move.from) << indexToSquare(move.to);
        if (move.promotion != 0) {
            //cout << (char)move.promotion;
        }
        //cout << " ----" << endl;
        //cout << "[TIME] White pieces: " << temp.whitePieces << endl;
        //cout << "[TIME] Black pieces: " << temp.blackPieces << endl;
        //cout << "[TIME] White attacks: " << temp.whiteAttacks << endl;
        //cout << "[TIME] Black attacks: " << temp.blackAttacks << endl;
        //cout << "[TIME] White King pos: " << to_string(temp.whiteKingPos) << ", " << temp.whiteKing << endl;
        //cout << "[TIME] Black King pos: " << to_string(temp.blackKingPos) << ", " << temp.blackKing << endl;
        //cout << endl;
        //cout << "[TIME] White pawns: " << temp.whitePawns << endl;
        //cout << "[TIME] Black pawns: " << temp.blackPawns << endl;
        //cout << "[TIME] White bishops: " << temp.whiteBishops << endl;
        //cout << "[TIME] Black bishops: " << temp.blackBishops << endl;
        //cout << "[TIME] White knights: " << temp.whiteKnights << endl;
        //cout << "[TIME] Black knights: " << temp.blackKnights << endl;
        //cout << "[TIME] White rooks: " << temp.whiteRooks << endl;
        //cout << "[TIME] Black rooks: " << temp.blackRooks << endl;
        //cout << "[TIME] White queens: " << temp.whiteQueens << endl;
        //cout << "[TIME] Black queens: " << temp.blackQueens << endl;
        //cout << endl;
        //cout << "[TIME] All pieces: " << temp.allPieces << endl;
        //cout << endl;
        //cout << flush;
        temp.make_move(move);
        if (!temp.is_king_in_check(!temp.whiteToMove)) { // check if own king is not in check
            legalMoves.push_back(move);
        }
    }

    return legalMoves;
}


string extractFen(const string& input) {
    if (input.rfind("initial startpos") == 0) {
        return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    }
    const string prefix = "initial ";
    size_t start = input.find(prefix);
    if (start == string::npos) return "";

    start += prefix.size();
    return input.substr(start);
}

int main() {
    string line;

    Board board;
    while (getline(cin, line)) {
        if (line == "uci") {
            cout << "id name NeptuneBot" << endl;
            cout << "id author Jupyter" << endl;
            cout << "uciok" << endl << flush;
        } else if (line == "isready") {
            cout << "readyok" << endl << flush;
        } else if (line.rfind("initial", 0) == 0) {
            auto start_time = chrono::high_resolution_clock::now();

            string fen = extractFen(line);
            board.setBB(fen);

            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::nanoseconds>(end_time-start_time);
            cout << "[TIME] Bitboards Generated in " << duration.count() << " ns" << "\n" << flush;
            cout << "initialok" << endl << flush;
        } else if (line.rfind("move", 0) == 0) {
            if (line == "move start") continue;
            string move = line.substr(5);

            uint8_t from = (move.substr(1, 1)[0] - '1') *8 + (move.substr(0, 1)[0] - 'a');
            uint8_t to = (move.substr(3, 1)[0] - '1') *8 + (move.substr(2, 1)[0] - 'a');

            bool isEnPassant = false;
            if (board.hasEnPassant()) {
                uint64_t pawns = board.whiteToMove ? board.whitePawns : board.blackPawns;
                isEnPassant = (to == board.getEnPassantTarget()) && ((pawns & bitMasks[from] != 0));
            }
            
            char promotion = 0;
            if (move.length() == 5) {
                promotion = move[4];
            }
            auto start_time = chrono::high_resolution_clock::now();
            board.make_move(Move(from, to, promotion, isEnPassant));
            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::nanoseconds>(end_time-start_time);
            cout << "[TIME] Moves Updated in " << duration.count() << " ns\n" << flush;
        } else if (line.rfind("legal", 0) == 0) {
            vector<Move> legalMoves = GenerateLegalMoves(board);
            cout << "Legal Moves: ";

            for (int i = 0; i < legalMoves.size(); i++) {
                cout << indexToSquare(legalMoves[i].from) << indexToSquare(legalMoves[i].from);
                if (legalMoves[i].promotion != 0) {
                    cout << (char)legalMoves[i].promotion;
                }
                cout << " ";
            }

            cout << flush;
        } else if (line.rfind("go", 0) == 0) {
            auto start_time = chrono::high_resolution_clock::now();
            vector<Move> legalMoves = GenerateLegalMoves(board);
            std::default_random_engine engine(std::chrono::system_clock::now().time_since_epoch().count());
            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::nanoseconds>(end_time-start_time);
            cout << "[TIME] " << legalMoves.size() << " Moves Generated in " << duration.count() << " ns\n" << flush;
            std::uniform_int_distribution<int> dist(0, legalMoves.size() - 1);
            int randomIndex = dist(engine);
            Move bestMove = legalMoves[randomIndex];
            cout << "bestmove " << indexToSquare(bestMove.from) << indexToSquare(bestMove.to);
            if (bestMove.promotion != 0) {
                cout << (char)bestMove.promotion;
            }
            board.make_move(bestMove);
            cout << endl << flush;
        } else if (line == "quit") {
            break;
        }
    }

    return 0;
}