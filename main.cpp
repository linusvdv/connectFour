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
#include "protocol.h"
#include "perft.h"


int main ()
{
    position pos;
/*    show_board(pos, true);
    do_move(pos, 2);
    do_move(pos, 9);
    do_move(pos, 3);
    show_board(pos, true);
    undo_move(pos, 3);
    undo_move(pos, 9);
    undo_move(pos, 2);
    show_board(pos, true);*/

    for (int i = 0; i <= 7; i++) {
        std::cout << i << " " << perft(pos, i) << std::endl;
    }
    do_move(pos,  3);
    do_move(pos,  0);
    do_move(pos,  2);
    do_move(pos,  6);
    do_move(pos, 10);
//    do_move(pos,  5);
    std::cout << std::endl << "search:" << std::endl;
    show_board(pos, false);
    for (int i = 1; i <= 15; i++) {
        std::tuple<int, int> search = alphabeta(pos, i, -20000, 20000);
        std::cout <<  "depth "     << i 
                  << " value: "    << std::get<0>(search) 
                  << " bestmove: " <<  std::get<1>(search) << std::endl;
    }
    protocol(pos, "position LN YR_RYYYRY_YRR_YR_RYY_YY_RRR_YR_RR__R__Y___ Y moves 2");
    show_board(pos, false);
    protocol(pos, "perft 10");
    return 0;
}

