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
            mv[i] = -1;
    }
    return mv;
}


void do_count (position &pos, int mv, int c)
{
    // update counter of connect four
    for (int i = 0; i < 4; i++) {
        for (int k = 3; k >= 0; k--) {
            pos.cross_count[i][c][k+1] |= pos.cross_count[i][c][k] & (cross_types[i] << mv >> 24);
            pos.cross_count[i][c][k] &= ~(cross_types[i] << mv >> 24);
        }
    }
}


void do_move (position &pos, int mv)
{
    // update position
    pos.board = (pos.board | (1LL << mv));
    pos.red = (pos.red | ((1LL << mv) & pos.color));
    pos.color = ~pos.color;

    do_count(pos, mv, pos.color & 1LL);
}


void undo_move (position &pos, int mv)
{
    // update counter of connect four
    int c = pos.color & 1LL;
    for (int i = 0; i < 4; i++) {
        for (int k = 0; k < 4; k++) {
            pos.cross_count[i][c][k] |= pos.cross_count[i][c][k+1] & (cross_types[i] << mv >> 24);
            pos.cross_count[i][c][k+1] &= ~(cross_types[i] << mv >> 24);
        }
    }

    // update position
    pos.color = ~pos.color;
    pos.red = (pos.red & (~((1LL << mv) & pos.color)));
    pos.board = (pos.board & (~(1LL << mv)));
}
