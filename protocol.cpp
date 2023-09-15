#include <array>
#include <cstdint>
#include <ostream>
#include <string>
#include <iostream>
#include <ctype.h>
#include <tuple>
#include <atomic>
#include <queue>
#include <condition_variable>

#include "types.h"
#include "movemaker.h"
#include "perft.h"
#include "drawboard.h"
#include "evaluation.h"
#include "search.h"
#include "transpositiontable.h"


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


bool LN_moves (position &pos, std::string input)
{
    if (input.find("moves ") != 0)
        return false;

    input.erase(0, std::string("moves").length());

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

            // if the position has already a connect four
            if (is_won(pos))
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


bool LN (position &pos, std::string input)
{
    clear_board(pos);

    // read board informations
    for (int i = 0; i < 42; i++) {
        switch (input[i]) {
            case 'R':
                // check if a piece is under the place
                if (i > 6 && (pos.board & (1LL << (i - 7))) == 0) {
                    return false;
                }
                pos.board |= 1LL << i;
                pos.red   |= 1LL << i;
                do_count(pos, i, 0);
                break;
            case 'Y':
                // check if a piece is under the place
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

    if (input[42] != ' ') {
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

    input.erase(0, 1);

    return LN_moves(pos, input);
}


void protocol (position &pos, std::string input, std::atomic<bool>& search_stop)
{
    // isready
    if (input == "isready") {
        std::cout << "readyok" << std::endl;
    }

    // newgame
    else if (input == "newgame") {
        TT_clear();
        clear_board(pos);
        std::cout << "started a new game" << std::endl;
    }

    // position
    else if (input.find("position LN ") == 0) {
        input.erase(0, std::string("position LN ").length());
        // 42 pieces, 1 space and 1 color of piece;
        if (input.length() >= 44) {
            if (LN(pos, input) == false) {
                clear_board(pos);
                std::cout << "Protocol position ERROR: Wrong position LN!" << std::endl;
            }
            else
                std::cout << "Set the position LN" << std::endl;
        }
        else {
            std::cout << "Protocol position ERROR: Not long enough position LN!" << std::endl;
        }
    }

    // start position
    else if (input.find("position start") == 0) {
        clear_board(pos);
        input.erase(0, std::string("position start").length());
        if (input.length() > 0) {
            if (input[0] == ' ') {
                input.erase(0, 1);
                if (LN_moves(pos, input) == false) {
                    clear_board(pos);
                    std::cout << "Protocol position ERROR: Position start has wrong moves!" << std::endl;
                }
                else
                    std::cout << "Set the starting position with the moves" << std::endl;
            }
            else
                std::cout << "Protocol position ERROR: Position start has wrong data!" << std::endl;
        }
        else
            std::cout << "Set the starting position" << std::endl;
    }

    else if (input.find("moves ") == 0) {
        if(LN_moves(pos, input))
            std::cout << "Set the moves" << std::endl;
        else
            std::cout << "Protocol moves ERROR: moves are not allowed" << std::endl;
    }

    // perft
    else if (input.find("perft ") == 0) {
        input.erase(0, std::string("perft ").length());
        int depth = 0;
        for (int i = 0; i < int(input.length()); i++) {
            if (isdigit(input[i])){
                depth *= 10;
                depth += int(input[i]) - int('0');
            }
            else {
                std::cout << "Protocol perft ERROR: Wrong perft depth! The depth is not made out of numbers!" << std::endl;
                return;
            }
        }
        if (depth > 10) {
            depth = 10;
            std::cout << "Protocol perft WARNING: perft depth to high. Uses depth 10 instead." << std::endl;
        }
        else if (depth == 0) {
            std::cout << "Protocol perft ERROR: perft depth 0!" << std::endl;
            return;
        }
        if (is_won(pos)) {
            std::cout << "Protocol perft ERROR: this position has already a connect four!" << std::endl;
            return;
        }
        std::cout << "Perft " << depth << ": " << perft(pos, depth) << std::endl;
    }

    // draw the board
    else if (input.find("board") == 0) {
        if (input.find("debug") != std::string::npos)
            if (input.find("debug") == 6 && input[5] == ' ') {
                show_board(pos, true);
                return;
            }
        if (input.length() == std::string("board").length()) {
            show_board(pos, false);
            return;
        }
        std::cout << "Protocol board WARNING: draws board without debug" << std::endl;
        show_board(pos, false);
    }

    // search
    else if (input.find("go ") == 0) {
        int total_depth = 0;
        if (input.find("depth ") == 3) {
            input.erase(0, std::string("go depth ").length());
            for (int i = 0; i < int(input.length()); i++) {
                if (isdigit(input[i])) {
                    total_depth *= 10;
                    total_depth += int(input[i]) - int('0');
                }
                else {
                    std::cout << "Protocol search ERROR: Wrong search depth! The depth is not made out of numbers!" << std::endl;
                    return;
                }
            }

        }
        else if (input.find("infinite") == 3) {
            total_depth = 128;
            if (input.length() > std::string("go infinite").length())
                std::cout << "Protocol search WARNING: go infinite has too much informations." << std::endl;
        }
        else {
            std::cout << "Protocol search ERROR: Command not found!" << std::endl;
            return;
        }
        
        if (is_won(pos)) {
            std::cout << "Protocol search ERROR: the board has already a connect four!" << std::endl;
            return;
        }

        search_result last_search={};
        uint64_t nodes = 0;
        for (int depth = 1; depth <= total_depth; depth++) {
            if (search_stop)
                break;
            search_result search = alphabeta(pos, depth, wdlm_struct{0, 0, 1000, -1}, wdlm_struct{1000, 0, 0, -1} , search_stop);

            // subtract one from mate
            if (search.wdlm.mate > 0)
                search.wdlm.mate -= 1;
            else if (search.wdlm.mate < 0)
                search.wdlm.mate += 1;

            // add the nodes together
            nodes += search.nodes;

            // for search_stop
            if (search_stop) {
                search = last_search;
                depth -= 1;
            }

            // prints the data
            // info
            if (!search_stop && depth < total_depth)
                std::cout << "info ";
            // general
            std::cout <<     "depth: "    << depth
                      << "\t  bestmove: " << search.bestmv
                      << "\t  win: "      << search.wdlm.win
                      << "\t  draw: "     << search.wdlm.draw
                      << "\t  loss: "     << search.wdlm.loss;
            // mate
            if (search.wdlm.mate != 0)
                std::cout << "\t  mate: " << search.wdlm.mate;
            std::cout << "\t  nodes: "    << nodes << std::endl;

            // for search_stop
            last_search = search;
        }
    }

    else {
        std::cout << "Protocol ERROR: Command not found!" << std::endl;
    }
}


void protocol_queue_clear (std::queue<std::string>& protocol_queue, std::atomic<bool>& pending_task)
{
    std::queue<std::string> empty;
    std::swap(protocol_queue, empty);
    pending_task = false;
}


void protocol_reader (std::queue<std::string>& protocol_queue, std::mutex& protocol_queue_mutex, std::condition_variable& protocol_cv, std::atomic<bool>& pending_task, std::atomic<bool>& search_stop, std::atomic<bool>& global_stop)
{
    std::string input;
    do {
        std::getline(std::cin, input);

        protocol_queue_mutex.lock();
        // stops the program
        if (input == "quit") {
            search_stop = true;
            global_stop = true;
            protocol_queue_clear(protocol_queue, pending_task);
        }
        // stops the running prosses and clear the queue
        else if (input == "stop") {
            search_stop = true;
            protocol_queue_clear(protocol_queue, pending_task);
        }
        // add to the protocol queue
        else {
            protocol_queue.push(input);
            pending_task = true;
        }
        protocol_queue_mutex.unlock();
        // notifies the main program
        protocol_cv.notify_one();
    } while (input != "quit");
}
