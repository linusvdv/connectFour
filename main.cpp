#include <algorithm>
#include <cstdint>
#include <iostream>
#include <bit>
#include <array>
#include <bitset>
#include <iterator>
#include <ostream>
#include <tuple>
#include <vector>
#include <string>

#include "types.h"
#include "movemaker.h"
#include "drawboard.h"
#include "evaluation.h"
#include "search.h"


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
    for (int i = 0; i <= 8; i++) {
        std::cout << i << " " << perft(pos, i) << std::endl;
    }
/*    do_move(pos,  3);
    do_move(pos, 10);
    do_move(pos,  4);
    do_move(pos,  0);*/
    std::cout << std::endl << "search:" << std::endl;
    show_board(pos);
    for (int i = 1; i <= 25; i++) {
        std::tuple<int, int> search = alphabeta(pos, 0, i, -20000, 20000);
        std::cout <<  "depth "     << i 
                  << " value: "    << std::get<0>(search) 
                  << " bestmove: " <<  std::get<1>(search) << std::endl;
    }
    return 0;
}

