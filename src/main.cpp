#include <string>
#include <istream>
#include <iostream>
#include <vector>
#include <sstream>
#include "board.h"
#include "boardsetup.h"
#include "movegen.h"

void uci() {
    std::cout << "id name Neptune\n";
    std::cout << "id author Jupyter\n";
    std::cout << "uciok\n" << std::flush;
}

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

int perft(Board& board, int depth) {
    int legalChildren = 0;
    if (depth == 0) return 1;
    std::vector<Move> legalMoves = GenerateLegalMoves(board);
    if (depth == 1) return static_cast<int>(legalMoves.size());
    for (const Move& move :legalMoves) {
        board.make_move(move);
        legalChildren += perft(board, depth-1);
        board.unmake_move();
    }
    return legalChildren;
}

int perft_divide(Board& board, int depth) {
    int total = 0;
    if (depth == 0) return 1;
    std::vector<Move> legalMoves = GenerateLegalMoves(board);
    for (const Move& move : legalMoves) {
        board.make_move(move);
        int nodes = perft(board, depth - 1);
        std::cout << MoveToUCI(move) << ": " << nodes << std::endl;
        total += nodes;
        board.unmake_move();
    }
    return total;
}

int main() {
    std::string line;
    Board board = Board();
    while (getline(std::cin, line)) {
        if (line == "uci") {
            uci();
        } else if (line == "isready") std::cout << "readyok\n" << std::flush;
        else if (line == "ucinewgame") {
            //Sent to indicate a new game is starting
            //Reset internal state if needed
            //No response required
        } else if (line.rfind("position", 0) == 0) {
            position(board, line);
        } else if (line.rfind("go", 0) == 0) {
            if (line.rfind("go perft", 0) == 0) {
                int depth = line.substr(0, 10)[9] - '0';
                int nodes = perft_divide(board, depth);
                std::cout << "Nodes searched: " << nodes << std::endl << std::flush;
            }
        } else if (line == "stop") {
            //Stop calculating as soon as possible
            //Must respond with bestmove of the best move found so far
        } else if (line == "quit") {
            //Exit the engine
        }
    }
}