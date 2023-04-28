#include <array>
#include <cstdint>
#include <bit>
#include <iostream>

#include "types.h"
#include "transpositiontable.h"
#include "drawboard.h"


bool is_won (position pos, int premv)
{
    uint64_t star_pos = (pos.board & (pos.red ^ pos.color));
    if (24 - premv > 0)
        star_pos <<= 24 - premv;
    else
        star_pos >>= premv - 24;
    star_pos &= cross[premv];
    for (int i = 0; i < 13; i++) {
        if ((star_pos & connect_four[i]) == connect_four[i])
            return true;
    }
    return false;
}


int evaluation (position pos)
{
    return TT_hash(pos) % 300;
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
    // std::cout << eval << std::endl;
    // show_board(pos);
    return eval;
}
