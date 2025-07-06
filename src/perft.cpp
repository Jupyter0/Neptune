#include "perft.h"

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

void perft_debug(Board& board, int depth) {
    auto start = std::chrono::high_resolution_clock::now();
    int nodes = perft_divide(board, depth);
    auto end = std::chrono::high_resolution_clock::now();

    double seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();

    std::cout << "\nNodes searched: " << nodes << "\n";
    std::cout << "Time: " << seconds << "s\n";
    std::cout << "Speed: " << static_cast<uint64_t>(nodes / seconds) << " NPS\n";

}