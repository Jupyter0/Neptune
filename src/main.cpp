#include "main.h"

int main() {
    std::string line;
    InitAttackTables();
    std::cout << "Neptune 1.0 by Jupyter" << std::endl << std::flush;
    Board board = Board();
    while (getline(std::cin, line)) {
        UCI(line, board);
    }
}