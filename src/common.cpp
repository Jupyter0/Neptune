#include "common.h"

std::string indexToSquare(uint8_t index) {
    uint8_t file = index & 7;  // 0 to 7
    uint8_t rank = index >> 3;  // 0 to 7

    char fileChar = static_cast<char>('a' + file);       // 'a' to 'h'
    char rankChar = static_cast<char>('1' + rank);       // '1' to '8'

    return std::string() + fileChar + rankChar;
}

Piece charToPiece(char pieceChar) {
    pieceChar = static_cast<char>(tolower(pieceChar));
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

uint8_t squareToIndex(std::string s) {
    int rank = s[1] - '1';
    int file = s[0] - 'a';
    if (rank >= 0 && rank < 64 && file >= 0 && file < 64) return static_cast<uint8_t>(rank*8+file);
    throw std::invalid_argument("Invalid Square: " + s);
};

Move parseUCIMove(const std::string& uci) {
    if (uci.length() < 4) {
        throw std::invalid_argument("Invalid UCI move: " + uci);
    }

    uint8_t from = squareToIndex(uci.substr(0, 2));
    uint8_t to = squareToIndex(uci.substr(2, 2));
    char promotion = 0;

    if (uci.length() == 5) {
        promotion = uci[4];
    }

    return Move(from, to, promotion);
}

std::vector<std::string> split(const std::string& s) {
    std::istringstream iss(s);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string MoveToUCI(const Move& move) {
    std::string uci = indexToSquare(move.from) + indexToSquare(move.to);
    if (move.promotion != 0) {
        uci += static_cast<char>(tolower(move.promotion));  // promotion should be lowercase
    }
    return uci;
}