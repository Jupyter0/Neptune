#include "boardsetup.h"

void position(Board& board, const std::string& input) {
    bool resetBoard = false;
    std::string fenString;
    std::vector<std::string> movesTokens;

    if (input.find("position startpos") == 0) {
        resetBoard = true;
        setBB(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");  // assume startingFEN is defined with standard start position FEN
    }
    else if (input.find("position fen ") == 0) {
        resetBoard = true;
        size_t fenStart = input.find("fen ") + 4;
        size_t movesPos = input.find(" moves ", fenStart);
        if (movesPos == std::string::npos) {
            fenString = input.substr(fenStart);
        } else {
            fenString = input.substr(fenStart, movesPos - fenStart);
        }
        setBB(board, fenString);
    }

    size_t movesIndex = input.find(" moves ");
    if (movesIndex != std::string::npos) {
        std::string movesPart = input.substr(movesIndex + 7);
        movesTokens = split(movesPart);
    }

    if (resetBoard) {
        board.appliedMoves.clear();
    }

    size_t firstNewMove = 0;
    while (firstNewMove < board.appliedMoves.size() && firstNewMove < movesTokens.size()) {
        std::string appliedMoveUCI = MoveToUCI(board.appliedMoves[firstNewMove]);
        if (appliedMoveUCI != movesTokens[firstNewMove]) break;
        firstNewMove++;
    }

    for (size_t i = firstNewMove; i < movesTokens.size(); ++i) {
        Move m = parseUCIMove(movesTokens[i]);
        board.make_move(m);
        board.appliedMoves.push_back(m);
    }
}

void ParsePieces(Board& board, std::string piecesField) {
    std::vector<std::string> ranks;
    std::stringstream ss(piecesField);
    std::string rank;
    while (getline(ss, rank, '/')) ranks.push_back(rank);

    std::memset(board.bitboards, 0, sizeof(board.bitboards));

    for (uint8_t i = 0; i < 8 ; i++) {
        uint8_t file = 0;
        for (uint8_t j = 0; j < ranks[i].length(); j++) {
            char piece = ranks[i][j];
            if (isdigit(piece)) {
                for (uint8_t k = 0; k < (piece - '0'); k++) {
                    board.pieceAt[static_cast<uint8_t>((7-i)*8 + file)] = EMPTY;
                    file++;
                }
                continue;
            }
            uint8_t squareIndex = static_cast<uint8_t>((7-i)*8 + file); // Board square index: a1 = 0, h8 = 63 (bottom-left to top-right)
            switch (piece) {
                    case 'P': board.bitboards[WHITE][0] |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = PAWN;   break;
                    case 'N': board.bitboards[WHITE][1] |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = KNIGHT; break;
                    case 'B': board.bitboards[WHITE][2] |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = BISHOP; break;
                    case 'R': board.bitboards[WHITE][3] |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = ROOK;   break;
                    case 'Q': board.bitboards[WHITE][4] |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = QUEEN;  break;
                    case 'K': board.bitboards[WHITE][5] |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = KING;   break;

                    case 'p': board.bitboards[BLACK][0] |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = PAWN;   break;
                    case 'n': board.bitboards[BLACK][1] |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = KNIGHT; break;
                    case 'b': board.bitboards[BLACK][2] |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = BISHOP; break;
                    case 'r': board.bitboards[BLACK][3] |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = ROOK;   break;
                    case 'q': board.bitboards[BLACK][4] |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = QUEEN;  break;
                    case 'k': board.bitboards[BLACK][5] |= bitMasks[squareIndex]; board.pieceAt[squareIndex] = KING;   break;
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
    board.enPassantSquare = 0;
    if (fields[3] != "-") {
        int file = fields[3][0] - 'a';
        int rank = fields[3][1] - '1';
        if (file >= 0 && file < 8 && rank >= 0 && rank < 8) {
            board.enPassantSquare = static_cast<uint8_t>(epFlag | (rank * 8 + file));
        } else {
            std::cerr << "[Warning] Invalid en passant square in FEN: " << fields[3] << std::endl;
        }
    }

    board.halfmoveClock = std::stoi(fields[4]);
    board.fullmoveNumber = std::stoi(fields[5]);

    board.UpdateOccupancy();
    UpdateAttacks(board);
}