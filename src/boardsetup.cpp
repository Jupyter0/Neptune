#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include "board.h"
#include "boardsetup.h"

void ParsePieces(Board& board, std::string piecesField) {
    std::vector<std::string> ranks;
    std::stringstream ss(piecesField);
    std::string rank;
    while (getline(ss, rank, '/')) ranks.push_back(rank);

    board.whitePawns = board.whiteKnights = board.whiteBishops = board.whiteRooks = board.whiteQueens = board.whiteKing = 0ULL;
    board.blackPawns = board.blackKnights = board.blackBishops = board.blackRooks = board.blackQueens = board.blackKing = 0ULL;

    for (uint8_t i = 0; i < 8 ; i++) {
        uint8_t file = 0;
        for (uint8_t j = 0; j < ranks[i].length(); j++) {
            char piece = ranks[i][j];
            if (isdigit(piece)) {
                for (int k = 0; k < (piece - '0'); k++) {
                    board.pieceAt[(7-i)*8 + file] = EMPTY;
                    file++;
                }
                continue;
            }
            uint8_t squareIndex = (7-i)*8 + file; // Board square index: a1 = 0, h8 = 63 (bottom-left to top-right)
            switch (piece) {
                    case 'P': board.whitePawns     |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = PAWN;   break;
                    case 'N': board.whiteKnights   |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = KNIGHT; break;
                    case 'B': board.whiteBishops   |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = BISHOP; break;
                    case 'R': board.whiteRooks     |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = ROOK;   break;
                    case 'Q': board.whiteQueens    |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = QUEEN;  break;
                    case 'K': board.whiteKing      |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = KING;   break;

                    case 'p': board.blackPawns     |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = PAWN;   break;
                    case 'n': board.blackKnights   |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = KNIGHT; break;
                    case 'b': board.blackBishops   |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = BISHOP; break;
                    case 'r': board.blackRooks     |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = ROOK;   break;
                    case 'q': board.blackQueens    |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = QUEEN;  break;
                    case 'k': board.blackKing      |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = KING;   break;
                    default: std::cerr << "[WARNING]: Unknown piece character '" << piece << "'\n"; break;
            }
            file++;
        }
    }
}

void setBB(Board& board, const std::string& fen) {
    if (fen.length() == 0) return;
    std::istringstream fss(fen);
    std::string field;
    std::vector<std::string> fields;

    while (fss >> field) {
        fields.push_back(field);
    }

    if (fields.size() < 6) {
        std::cerr << "Invalid FEN: not enough fields\n";
        return;
    }

    ParsePieces(board, fields[0]);

    board.whiteToMove = (fields[1] == "w");

    board.castlingRights = 0ULL;
    if (fields[2] != "-") {
        for (char c : fields[2]) {
            switch (c) {
                case 'K': board.castlingRights |= 1 << 3; break;
                case 'Q': board.castlingRights |= 1 << 2; break;
                case 'k': board.castlingRights |= 1 << 1; break;
                case 'q': board.castlingRights |= 1 << 0; break;
            }
        }
    }

    constexpr uint8_t epFlag = 1 << 6;
    board.enPassantSquare = 0ULL;
    if (fields[3] != "-") {
        int file = fields[3][0] - 'a';
        int rank = fields[3][1] - '1';
        if (file >= 0 && file < 8 && rank >= 0 && rank < 8) {
            board.enPassantSquare = epFlag | (rank * 8 + file);
        } else {
            std::cerr << "[Warning] Invalid en passant square in FEN: " << fields[3] << std::endl;
        }
    }

    board.halfmoveClock = std::stoi(fields[4]);
    board.fullmoveNumber = std::stoi(fields[5]);

    board.UpdateOccupancy();
    board.UpdateAttacks();
}