#include <array>
#include <cstdint>
#include <iostream>

#include "types.h"
#include "evaluation.h"
#include "movemaker.h"
#include "drawboard.h"


int alphabeta (position pos, int premv, int depth, int alpha, int beta)
{
    if (is_won(pos, premv) == true) {
        if (pos.color == 0) {
            return -10000;
        }
        else
            return  10000;
    }

    if (depth == 0)
        return evaluation(pos, premv);


    int bestvalue = alpha;

    std::array<int, 7> mv = moves(pos);

    // move sorter

    for (int i = 0; i < 7; i++) {
        if (mv[i] == -1LL) {
            continue;
        }

        do_move(pos, mv[i]);
        int value = -alphabeta(pos, mv[i], depth-1, -beta, -bestvalue);
/*        if (depth == 5 || true) {
            std::cout << std::endl << value << " " << bestvalue << std::endl;
            show_board(pos);
        }*/
        undo_move(pos, mv[i]);


        if (value > bestvalue) {
            bestvalue = value;
            if (bestvalue >= beta)
                break;
        }
    }

    return bestvalue;
}
