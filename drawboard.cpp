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


void show_board (position pos, bool debug)
{
    std::cout << std::endl;
    for (int i = 5; i >= 0; i--) {
        for (int j = 0; j < 7; j++) {
            int color = ((pos.board & (1LL << (i*7 + j)))
                       + (pos.red   & (1LL << (i*7 + j))))
                      >> (i*7 + j);
            std::cout << colorize(color) << "O";
        }
        if (i == 5)
            std::cout << colorize(0) << " "
                      << colorize(int(pos.color & 1LL) + 1) << " ";
        std::cout << colorize(0) << std::endl;
    }
    std::cout << std::endl;
    if (debug) {
        for (int n = 0; n < 2; n++)
            for (int m = 0; m < 4; m++) {
                for (int i = 5; i >= 0; i--) {
                    for (int o = 0; o < 5; o++) {
                        for (int j = 0; j < 7; j++) {
                            int color = 1LL & (pos.cross_count[m][n][o] >> (i*7 + j));
                            color *= 2-n;
                            std::cout << colorize(color) << "O";
                        }
                        std::cout << colorize(0) << " ";
                    }
                    std::cout << colorize(0) << std::endl;
                }
                std::cout << std::endl;
            }
    }
}
