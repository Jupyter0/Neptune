#ifndef UCI_H
#define UCI_H

#include "common.h"
#include "boardsetup.h"
#include "perft.h"
#include "search.h"

void UCI(std::string line, Board& board);

#endif