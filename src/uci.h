#ifndef UCI_H
#define UCI_H

#include "common.h"
#include "boardsetup.h"
#include "perft.h"
#include "search.h"
#include "transpositiontable.h"

void UCI(std::string line, Board& board);

#endif