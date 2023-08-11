#include <array>
#include <cstdint>
#include <bit>
#include <iostream>
#include <cmath>

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


wdlm_struct evaluation (position pos)
{
    float eval = 0;
    // Here will be the evaluation eventually the net

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 2; j++)
            for (int k = 1; k < 5; k++) {
                eval += (std::popcount(pos.cross_count[i][  j][k] & 
                                       pos.cross_count[i][1-j][0]) << (k*2)) * (j*2-1);
            }
//    show_board(pos, false);
//    std::cout << 50 - 50 * erff(eval / 100) << std::endl;
    return wdlm_struct{50 + 50 * erff(eval / 100), 0, 50 - 50 * erff(eval / 100), -1};
}
