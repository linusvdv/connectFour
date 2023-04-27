#include "types.h"


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


int evaluation ([[maybe_unused]]position pos, [[maybe_unused]]int premv)
{
    // Here will be the evaluation eventually the net
    return 0;
}
