#ifndef MAGICBITBOARDS_H
#define MAGICBITBOARDS_H

#include "common.h"

extern uint64_t rookAttackTable[64][4096];
extern uint64_t rookBlockerMask[64];
extern uint64_t rookMagic[64];
extern int rookShift[64];

extern uint64_t bishopAttackTable[64][512];
extern uint64_t bishopBlockerMask[64];
extern uint64_t bishopMagic[64];
extern int bishopShift[64];

void InitAttackTables();

#endif