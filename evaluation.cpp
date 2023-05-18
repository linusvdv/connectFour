#include <array>
#include <cstdint>
#include <bit>
#include <iostream>

#include "types.h"
#include "transpositiontable.h"
#include "drawboard.h"


bool is_won (position pos)
{
    uint64_t x = 0;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 2; j++)
            x |= pos.cross_count[i][j][4];
    return std::popcount(x) > 0;
}


int evaluation (position pos)
{
    int eval = 0;
    // Here will be the evaluation eventually the net

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 2; j++)
            for (int k = 0; k < 5; k++) {
                eval += (std::popcount(pos.cross_count[i][  j][k] & 
                                       pos.cross_count[i][1-j][0]) << (k*2)) * (j*2-1);
            }
//    if (pos.color == uint64_t(-1))
//        eval += 100;
    return eval;
}
