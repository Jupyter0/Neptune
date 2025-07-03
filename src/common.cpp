#include <string>
#include <stdexcept>
#include <vector>
#include <sstream>
#include "common.h"

std::string indexToSquare(int index) {
    int file = index & 7;  // 0 to 7
    int rank = index >> 3;  // 0 to 7

    char fileChar = 'a' + file;       // 'a' to 'h'
    char rankChar = '1' + rank;       // '1' to '8'

    return std::string() + fileChar + rankChar;
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

Move parseUCIMove(const std::string& uci) {
    if (uci.length() < 4) {
        throw std::invalid_argument("Invalid UCI move: " + uci);
    }

    auto squareToIndex = [](const std::string& s) -> int {
        return (s[0] - 'a') + (s[1] - '1') * 8;
    };

    int from = squareToIndex(uci.substr(0, 2));
    int to = squareToIndex(uci.substr(2, 2));
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