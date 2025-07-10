#include "perft.h"

uint64_t perft(Board& board, int depth) {
    uint64_t legalChildren = 0;
    if (depth == 0) return 1;
    Move moves[256];
    int pseudoMoveCount = GeneratePseudoLegalMoves(board, moves);
    for (int i = 0; i < pseudoMoveCount; ++i) {
        Move move = moves[i];
        board.make_move(move);
        if (board.is_king_in_check(!board.whiteToMove)) { board.unmake_move(); continue; }
        legalChildren += perft(board, depth-1);
        board.unmake_move();
    }
    return legalChildren;
}

uint64_t perft_divide(Board& board, int depth) {
    if (depth == 0) return 1;
    Move moves[256];
    int pseudoMoveCount = GeneratePseudoLegalMoves(board, moves);
    std::atomic<int> index(0);
    std::atomic<uint64_t> totalNodes = 0;
    std::mutex cout_mutex;

    auto worker = [&]() {
        while(true) {
            int i = index.fetch_add(1);
            if (i >= pseudoMoveCount) break;

            const Move& move = moves[i];
            Board copy = board;
            copy.make_move(move);
            if (copy.is_king_in_check(!copy.whiteToMove)) continue;

            uint64_t nodes = perft(copy, depth - 1);
            totalNodes.fetch_add(nodes, std::memory_order_relaxed);

            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << MoveToUCI(move) << ": " << nodes << '\n';
        }
    };

    std::vector<std::thread> threads;
    for (uint i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker);
    }
    for (auto& t : threads) t.join();

    return totalNodes;
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