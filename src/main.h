#ifndef MAIN_H
#define MAIN_H
#include <string>
#include "board.h"

void uci();
void position(Board& board, const std::string& input);
void main();

#endif