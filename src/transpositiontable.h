#ifndef TRANSPOSITIONTABLE_H
#define TRANSPOSITIONTABLE_H

#include "common.h"

using namespace NeptuneInternals;

enum EvalFlag : uint8_t {
    EXACT, LOWERBOUND, UPPERBOUND
};

struct TTEntry {
    uint64_t zobristKey;
    int depth;
    int eval;
    EvalFlag flag;
    Move move;
};

class TranspositionTable {
    std::vector<TTEntry> table;  // fixed size (e.g., 2^25 entries)
    size_t sizeMask;             // usually table.size() - 1

public:
    TranspositionTable(size_t sizePowerOfTwo) {
        table.resize(1 << sizePowerOfTwo);
        sizeMask = table.size() - 1;
    }

    TTEntry* probe(uint64_t zobristKey) {
        return &table[zobristKey & sizeMask];
    }

    void store(uint64_t zobristKey, TTEntry entry) {
        TTEntry* slot = probe(zobristKey);
        // Replace based on depth or age
        if (entry.depth > slot->depth /* or other conditions */) {
            *slot = entry;
        }
    }
};

#endif