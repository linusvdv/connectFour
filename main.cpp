#include <algorithm>
#include <cstdint>
#include <iostream>
#include <bit>
#include <array>
#include <bitset>
#include <iterator>
#include <ostream>
#include <vector>
#include <string>

#include "types.h"
#include "movemaker.h"
#include "drawboard.h"
#include "evaluation.h"


uint64_t perft (position &pos, int depth)
{
    if (depth == 0)
        return 1;
    uint64_t nodes = 0;
    std::array<int, 7> mv = moves(pos);
    for (int i = 0; i < 7; i++) {
        if (mv[i] == -1LL) {
            continue ;
        }
        do_move(pos, mv[i]);
        if (is_won(pos, mv[i]) == false)
            nodes += perft(pos, depth - 1);
        else {
            nodes += 1;
        }
        undo_move(pos, mv[i]);
    }
    if (nodes == 0)
        return 1;
    return nodes;
}


int main ()
{
    position pos;
    show_board(pos);
    for (int i = 0; i <= 10; i++) {
        std::cout << i << " " << perft(pos, i) << std::endl;
    }
    return 0;
}

