#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <iostream>

#include "types.h"
#include "transpositiontable.h"
#include "movemaker.h"


bool TT_move_sorter (position pos, std::array<int, 7>& mv)
{
    TT_result TT_data = TT_get(pos);
    if (TT_data.TT_hit) {
        int column = TT_data.mv % 7;
        if (column != 0)
            std::swap(mv[0], mv[column]);
        return true;
    }
    return false;
}


void move_sorter ([[maybe_unused]]position pos, std::array<int, 7>& mv, bool TT_move)
{
    std::array<std::pair<int, int>, 7> mv_value = {};
    for (int i = 0; i < 7; i++) {
        mv_value[i].second = mv[i];
        if (i == 0 && TT_move) {
            mv_value[i].first = 10000;
        }
        else {
            mv_value[i].first += std::min((mv[i] % 7), (7 - (mv[i] % 7)));
        }
    }
    // sort
    std::sort(mv_value.begin(), mv_value.end(), std::greater< std::pair<int, int> >() );

    for (int i = 0; i < 7; i++) {
        mv[i] = mv_value[i].second;
    }
}
