#ifndef ATTACKGEN_H
#define ATTACKGEN_H

//Deprecated

#include "common.h"
#include "board.h"
#include "magicbitboards.h"

/**
 * @warning DEPRECATED, its slow, use board.isSquareAttacked() instead.
 */
void UpdateAttacks(Board& board);

#endif