#include "types.h"


bool is_won(position pos, int mv)
{
    uint64_t star_pos = (pos.board & (pos.red ^ pos.color));
    if (24 - mv > 0)
        star_pos <<= 24 - mv;
    else
        star_pos >>= mv - 24;
    star_pos &= cross[mv];
    for (int i = 0; i < 13; i++) {
        if ((star_pos & connect_four[i]) == connect_four[i])
            return true;
    }
    return false;
}

int evaluation(position pos, int mv)
{
    if (is_won(pos, mv) == true) {
        if (pos.color == 0)
            return  10000;
        else
            return -10000;
    }
    return 0;
}
