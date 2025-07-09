#include "perft.h"

uint64_t perft(Board& board, int depth) {
    uint64_t legalChildren = 0;
    if (depth == 0) return 1;
    std::vector<Move> legalMoves = GenerateLegalMoves(board);
    if (depth == 1) return static_cast<uint64_t>(legalMoves.size());
    for (const Move& move :legalMoves) {
        board.make_move(move);
        legalChildren += perft(board, depth-1);
        board.unmake_move();
    }
    return legalChildren;
}

uint64_t perft_divide(Board& board, int depth) {
    uint64_t total = 0;
    if (depth == 0) return 1;
    std::vector<Move> legalMoves = GenerateLegalMoves(board);
    for (const Move& move : legalMoves) {
        board.make_move(move);
        uint64_t nodes = perft(board, depth - 1);
        std::cout << MoveToUCI(move) << ": " << nodes << std::endl;
        total += nodes;
        board.unmake_move();
    }
    return total;
}

void perft_debug(Board& board, int depth) {
    auto start = std::chrono::high_resolution_clock::now();
    uint64_t nodes = perft_divide(board, depth);
    auto end = std::chrono::high_resolution_clock::now();

    uint64_t micros = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
    uint64_t nps = static_cast<uint64_t>((static_cast<__uint128_t>(nodes)*1'000'000) / micros);

    std::cout << "\nNodes searched: " << nodes << "\n";
    std::cout << "Time: " << static_cast<double>(micros) / 1'000'000.0 << "s\n";
    std::cout << "Speed: " << static_cast<uint64_t>(nps) << " NPS\n";
}