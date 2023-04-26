#include <array>
#include <bit>

#include "types.h"


std::array<int, 7> moves (position pos)
{
    // get all moves
    std::array<int, 7> mv = {};
    for (int i = 0; i < 7; i++) {
        // if the columns is not full
        if ((pos.board & columns[i]) != columns[i])
            // add this position
            mv[i] = std::countr_zero((~pos.board) & columns[i]);
        else
            mv[i] = -1LL;
    }
    return mv; 
}


void do_move (position &pos, int mv)
{
    pos.board = (pos.board | (1LL << mv));
    pos.red = (pos.red | ((1LL << mv) & pos.color));
    pos.color = ~pos.color;
}


void undo_move (position &pos, int mv)
{
    pos.color = ~pos.color;
    pos.red = (pos.red & (~((1LL << mv) & pos.color)));
    pos.board = (pos.board & (~(1LL << mv)));
}
