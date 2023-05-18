#include <array>
#include <string>
#include <iostream>
#include <ctype.h>

#include "types.h"
#include "movemaker.h"
#include "perft.h"
#include "drawboard.h"


void clear_board (position &pos)
{
    pos.board =  0;
    pos.red   =  0;
    pos.color = -1;
    pos.cross_count = {{
        {{{519454312335, 0, 0, 0, 0}, {519454312335, 0, 0, 0, 0}}},
        {{{2097151, 0, 0, 0, 0}, {2097151, 0, 0, 0, 0}}},
        {{{247695, 0, 0, 0, 0}, {247695, 0, 0, 0, 0}}},
        {{{1981560, 0, 0, 0, 0}, {1981560, 0, 0, 0, 0}}}
    }};
}


bool LN (position &pos, std::string input)
{
    clear_board(pos);

    // read board informations
    for (int i = 0; i < 42; i++) {
        switch (input[i]) {
            case 'R':
                if (i > 6 && (pos.board & (1LL << (i - 7))) == 0) {
                    return false;
                }
                pos.board |= 1LL << i;
                pos.red   |= 1LL << i;
                do_count(pos, i, 0);
                break;
            case 'Y':
                if (i > 6 && (pos.board & (1LL << (i - 7))) == 0) {
                    return false;
                }
                pos.board |= 1LL << i;
                do_count(pos, i, 1);
                break;
            case '_':
                break;
            default:
                return false;
        }
    }

    if (input[42]!=' ') {
        return false;
    }

    // color to move
    switch (input[43]) {
        case 'R':
            pos.color = -1;
            break;
        case 'Y':
            pos.color = 0;
            break;
        default:
            return false;
    }

    input.erase(0, 44);

    if (input.length() == 0)
        return true;
  
    if (input[0] != ' ')
        return false;

    if (input.find("moves ") == std::string::npos)
        return false;

    input.erase(0, input.find("moves ") + std::string("moves").length());

    // do moves
    int mv_num = 0;
    for (int i = 1; i < int(input.length()); i++) {
        // convert to nuber
        if (isdigit(input[i])) {
            mv_num *= 10;
            mv_num += int(input[i]) - int('0');
        }
        else if (input[i] == ' ') {
            if (input[i-1] == ' ')
                return false;

            // check if move is leagel
            std::array<int, 7> leagal_moves = moves(pos);
            if (leagal_moves[mv_num%7] != mv_num || mv_num == -1)
                return false;
            do_move(pos, mv_num);
            mv_num = 0;
        }
        else
            return false;
    }

    if (input[input.length()] == ' ')
        return false;

    // check if the last move is a leagal move
    std::array<int, 7> leagal_moves = moves(pos);
    if (leagal_moves[mv_num%7] != mv_num || mv_num == -1)
        return false;
    do_move(pos, mv_num);
    return true;
}


void protocol (position &pos, std::string input)
{
    // position
    if (input.find("position LN ") != std::string::npos) {
        input.erase(0, input.find("position LN ") + std::string("position LN ").length());
        // 42 pieces, 1 space and 1 color of piece;
        if (input.length() >= 44) {
            if (LN(pos, input) == false) {
                clear_board(pos);
                std::cout << "Protocol ERROR: Wrong position LN!" << std::endl;
            }
        }
        else {
            std::cout << "Protocol ERROR: Not long enough position LN!" << std::endl;
        }
    }

    // perft
    else if (input.find("perft ") != std::string::npos) {
        input.erase(0, input.find("perft ") + std::string("perft ").length());
        int depth = 0;
        for (int i = 0; i < int(input.length()); i++) {
            if (isdigit(input[i])){
                depth *= 10;
                depth += int(input[i]) - int('0');
            }
            else {
                std::cout << "Protocol ERROR: Wrong perft depth! \
                              The depth is not made out of numbers!" << std::endl;
                return;
            }
        }
        if (depth > 10) {
            depth = 10;
            std::cout << "Protocol WARNING: perft depth to high. Uses depth 10 instead." << std::endl;
        }
        else if (depth == 0) {
            std::cout << "Protocol ERROR: perft depth 0" << std::endl;
            return;
        }
        std::cout << "Perft " << depth << ": " << perft(pos, depth) << std::endl;
    }

    else if (input.find("board") != std::string::npos) {
        if (input.length() == std::string("Board").length()) {
            show_board(pos, false);
            return;
        }
        if (input.find("debug") != std::string::npos) {
            if (input.find("debug") == 6 && input[5] == 0)
                show_board(pos, true);
        }
        else {
            std::cout << "Protocol WARNING: draws board without debug";
            show_board(pos, false);
        }
    }
}
