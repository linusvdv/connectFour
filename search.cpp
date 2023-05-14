#include <array>
#include <bit>
#include <cstdint>
#include <iostream>
#include <tuple>

#include "types.h"
#include "evaluation.h"
#include "movemaker.h"
#include "drawboard.h"
#include "movesorter.h"
#include "transpositiontable.h"


std::tuple<int, int> alphabeta (position pos, int depth, int alpha, int beta)
{
    if (is_won(pos) == true) {
        if (pos.color == 0) {
            return std::make_tuple(-10000, -1);
        }
        else
            return std::make_tuple( 10000, -1);
    }

    if (depth <= 0 || std::popcount(pos.board) >= 42)
        return std::make_tuple(evaluation(pos), -1);


    int bestvalue = alpha;
    int bestmv = -1;

    std::array<int, 7> mv = moves(pos);

    // move sorter
    bool TT_mv = TT_move_sorter(pos, mv);
    if (TT_mv == true) {
        TT_result TT_data = TT_get(pos);
        if (TT_data.depth >= depth)
            return std::make_tuple(TT_data.value, TT_data.mv);
    }
    move_sorter(pos, mv, TT_mv);

    // search
    for (int i = 0; i < 7; i++) {
        if (mv[i] == -1LL) {
            continue;
        }
        int value = 0;
        do_move(pos, mv[i]);
        if (TT_mv && i == 0)
            value = -std::get<0>(alphabeta(pos, depth-1, -beta, -bestvalue));
        else
            value = -std::get<0>(alphabeta(pos, depth-2, -beta, -bestvalue));
        undo_move(pos, mv[i]);


        if (value > bestvalue) {
            bestvalue = value;
            bestmv = mv[i];
            if (bestvalue >= beta)
                break;
        }
    }

    // add to TT
    if (bestmv != -1)
        TT_set(pos, depth, bestvalue, bestmv);
    return std::make_tuple(bestvalue, bestmv);
}
