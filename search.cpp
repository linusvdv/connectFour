#include <array>
#include <bit>
#include <cstdint>
#include <iostream>
#include <tuple>
#include <atomic>

#include "types.h"
#include "evaluation.h"
#include "movemaker.h"
#include "drawboard.h"
#include "movesorter.h"
#include "transpositiontable.h"


// swaps wdlm win with wdlm draw
// negates the mate depth
wdlm_struct wdlm_reverse (wdlm_struct wdlm)
{
    return wdlm_struct{wdlm.loss, wdlm.draw, wdlm.win, -wdlm.mate};
}


// increases the mate score by one if there is a mate
wdlm_struct mate_increase (wdlm_struct wdlm)
{
    if (wdlm.mate > 0)
        wdlm.mate += 1;
    else if (wdlm.mate < 0)
        wdlm.mate -= 1;
    return wdlm;
}


// decreases the mate score by one if there is a mate
wdlm_struct mate_decrease (wdlm_struct wdlm)
{
    if (wdlm.mate > 0)
        wdlm.mate -= 1;
    else if (wdlm.mate < 0)
        wdlm.mate += 1;
    return wdlm;
}

// >= or >
bool wdlm_ge (wdlm_struct wdlm1, wdlm_struct wdlm2, bool equal)
{
    // even
    if ((wdlm1.win - wdlm1.loss) == (wdlm2.win - wdlm2.loss)) {
        // no mate
        if (wdlm1.mate == 0 && wdlm2.mate == 0)
            return equal;
        // both are good for the same
        else if (wdlm1.mate * wdlm2.mate > 0) {
            if (equal)
                return (wdlm1.mate <= wdlm2.mate);
            else
                return (wdlm1.mate < wdlm2.mate);
        }
        // one is higher than the other
        else {
            return (wdlm1.mate > wdlm2.mate);
        }
    }
    else {
        return ((wdlm1.win - wdlm1.loss) > (wdlm2.win - wdlm2.loss));
    }

}


// search
search_result alphabeta (position pos, int depth, wdlm_struct alpha, wdlm_struct beta, std::atomic<bool>& search_stop)
{
    // search stop
    if (search_stop)
        return search_result{wdlm_struct{0, 0, 0, 0}, -1, 0, true};

    // connect four
    if (is_won(pos) == true)
        return search_result{wdlm_struct{0, 0, 100, -1}, -1, 1, false};

    // all pices on the board
    if (std::popcount(pos.board) >= 42)
        return search_result{wdlm_struct{0, 100, 0, 0}, -1, 1, false};

    // transpositiontable
    TT_result TT_data = TT_get(pos);
    if (TT_data.TT_hit && TT_data.depth >= depth)
        return search_result{TT_data.wdlm, TT_data.mv, 1, false};

    // evaluation
    if (depth <= 0)
        return search_result{evaluation(pos), -1, 1, false};

    // get leagal moves
    std::array<int, 7> mv = moves(pos);

    // move sorter
    move_sorter(pos, mv, TT_data);

    // search
    wdlm_struct bestvalue = alpha;
    int bestmv = -1;
    uint64_t nodes = 0;

    for (int i = 0; i < 7; i++) {
        // line is already full
        if (mv[i] == -1) {
            continue;
        }

        // next depth
        wdlm_struct wdlm_value = {};
        do_move(pos, mv[i]);
        search_result search_value = alphabeta(pos, depth-1, wdlm_reverse(mate_decrease(beta)), 
                                               wdlm_reverse(mate_decrease(bestvalue)), search_stop);
        wdlm_value = wdlm_reverse(mate_increase(search_value.wdlm));
        nodes += search_value.nodes;
        undo_move(pos, mv[i]);

        // search stop
        if (search_value.stop)
            return search_result{wdlm_struct{0, 0, 0, 0}, -1, 0, true};

        // best move
        if (wdlm_ge(wdlm_value, bestvalue, false)) {
            bestvalue = wdlm_value;
            bestmv = mv[i];
            if (wdlm_ge(bestvalue, beta, true))
                break;
        }
    }

    // add to TT if there is no cutoff
    if (bestmv != -1 && !wdlm_ge(bestvalue, beta, true))
        TT_set(pos, depth, bestvalue, bestmv);

    // return the value
    return search_result{bestvalue, bestmv, nodes, false};
}
