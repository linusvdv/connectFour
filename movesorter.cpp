#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <iostream>

#include "types.h"
#include "transpositiontable.h"
#include "movemaker.h"


void move_sorter ([[maybe_unused]]position pos, std::array<int, 7>& mv, TT_result TT_data)
{
    // pair of mv and the value of the move
    std::array<std::pair<int, int>, 7> mv_value = {};
    // TT hit
    if (TT_data.TT_hit) {
        int column = TT_data.mv % 7;
        if (column != 0)
            std::swap(mv[0], mv[column]);
        mv_value[0].second = mv[0];
    }

    // go throug the rest of the moves
    for (int i = (TT_data.TT_hit==true ? 1 : 0); i < 7; i++) {
        mv_value[i].second = mv[i];
        // distance to the middle of the board
        mv_value[i].first += std::min((mv[i] % 7), (6 - (mv[i] % 7)));
    }

    // sort
    std::sort(mv_value.begin(), mv_value.end(), std::greater< std::pair<int, int> >() );

    for (int i = 0; i < 7; i++) {
        mv[i] = mv_value[i].second;
    }
}
