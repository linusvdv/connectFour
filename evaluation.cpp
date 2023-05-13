#include <array>
#include <cstdint>
#include <bit>
#include <iostream>

#include "types.h"
#include "transpositiontable.h"
#include "drawboard.h"


bool is_won (position pos)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 2; j++)
            if (std::popcount(pos.cross_count[i][j][4]) > 0)
                return true;
    return false;
}


int evaluation (position pos)
{
    return 0;

    int eval = 0;
    // Here will be the evaluation eventually the net

    std::array<uint64_t, 2> pos_colors = {
        pos.red,
        (pos.board & (~pos.red))
    };
    int side = -1;
    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < 42; i++){
            uint64_t pos_color = pos_colors[j];
            if (24 - i > 0)
                pos_color <<= 24 - i;
            else
                pos_color >>= i - 24;
            pos_color &= cross[i];
            eval += side * (1 << std::popcount(pos_color));
        }
        side = 1;
    }
    return eval;
}
