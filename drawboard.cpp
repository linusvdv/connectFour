#include <string>
#include <iostream>

#include "types.h"


std::string colorize (int color)
{
    std::string reset = "\033[0m";
    std::string red = "\033[48;2;255;0;0m";
    std::string yellow = "\033[48;2;255;255;0m";
    if (color == 1)
        return yellow;
    else if (color == 2)
        return red;
    else if (color == 0)
        return reset;
    return "ERROR";
}


void show_board (position pos)
{
    for (int i = 5; i >= 0; i--) {
        for (int j = 0; j < 7; j++) {
            int color = ((pos.board & (1LL << (i*7 + j)))
                       + (pos.red   & (1LL << (i*7 + j))))
                      >> (i*7 + j);
            std::cout << colorize(color) << j;
        }
        if (i == 5)
            std::cout << colorize(0) << " "
                      << colorize(int(pos.color & 1LL) + 1) << " ";
        std::cout << colorize(0) << std::endl;
    }
    std::cout << std::endl;
}
