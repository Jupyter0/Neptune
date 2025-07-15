#include "uci.h"

using PVLine = std::vector<Move>;

void UCI(std::string line, Board& board) {
    if (line == "uci") {
        std::cout << "id name Neptune\n";
        std::cout << "id author Jupyter\n";
        std::cout << "option name Threads type spin default 1 min 1 max " << std::min<uint>(std::thread::hardware_concurrency(), 20) << "\n";
        std::cout << "uciok\n" << std::flush;
    } else if (line == "isready") std::cout << "readyok\n" << std::flush;
    else if (line == "ucinewgame") {
        //Sent to indicate a new game is starting
        //Reset internal state if needed
        //No response required
    } else if (line.rfind("position", 0) == 0) {
        Position(board, line);
    } else if (line.rfind("go", 0) == 0) {
        if (line.rfind("go perft", 0) == 0) {
            int depth = line.substr(0, 10)[9] - '0';
            PerftDebug(board, depth);
        } else {
            PVLine bestLine;
            int eval = MiniMax(board, 9, board.whiteToMove, bestLine);
            std::cout << "Eval: " << eval << "\nBest line:\n";
            for (const Move& move : bestLine) {
                std::cout << MoveToUCI(move) << " ";
            }
            std::cout << "\n";
            std::cout << "bestmove " << MoveToUCI(bestLine[0]);
            std::cout << "\n" << std::flush;
        }
    } else if (line.rfind("setoption", 0) == 0) {
        std::istringstream iss(line);
        std::string token;
        std::string name;
        std::string value;

        // skip "setoption"
        iss >> token;

        // parse until "name"
        iss >> token;
        if (token == "name") {
            // read name until "value" token or end
            std::string temp;
            while (iss >> temp) {
                if (temp == "value") break;
                if (!name.empty()) name += " ";
                name += temp;
            }

            // now if last read temp == "value", parse the value rest of line
            if (temp == "value") {
                std::getline(iss, value);
                // trim leading spaces from value
                value.erase(0, value.find_first_not_of(" "));
            }
        }

        if (name == "Threads") {
            try {
                int threads = std::stoi(value);
                // Clamp threads between 1 and hardware concurrency max
                uint maxThreads = std::min<uint>(std::thread::hardware_concurrency(), 20u);
                if (threads < 1) threads = 1;
                uint uthreads = static_cast<uint>(threads);
                if (uthreads > maxThreads) uthreads = maxThreads;
                numThreads = uthreads;
                std::cout << "info string Threads set to " << numThreads << "\n" << std::flush;
            } catch (...) {
                std::cerr << "info string Invalid Threads value: " << value << "\n" << std::flush;
            }
        }
    } else if (line == "stop") {
        //Stop calculating as soon as possible
        //Must respond with bestmove of the best move found so far
    } else if (line == "quit") {
        //Exit the engine
    }
}