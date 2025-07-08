#ifndef MAIN_H
#define MAIN_H

#include "common.h"
#include "board.h"
#include "boardsetup.h"
#include "perft.h"
#include "magicbitboards.h"

void uci();
void position(Board& board, const std::string& input);
int main();

#endif