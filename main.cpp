#include <algorithm>
#include <cstdint>
#include <iostream>
#include <bit>
#include <array>
#include <bitset>
#include <iterator>
#include <ostream>
#include <tuple>
#include <unistd.h>
#include <vector>
#include <string>
#include <atomic>
#include <future>
#include <chrono>
#include <mutex>

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
    std::atomic<bool> search_stop = false;
    std::string next_protocol_str = ""; // protected by next_protocol_mtx
    std::mutex next_protocol_mtx;

    // loop
    std::cout << "Connect Four by linusvdv" << std::endl;
    std::future<void> fut = std::async(std::launch::async, protocol, std::ref(pos), "position start", std::ref(search_stop), std::ref(next_protocol_str), std::ref(next_protocol_mtx));
    bool stop = false;
    while (!stop) {
        std::string input;
        std::getline(std::cin, input);

        // stops the program
        if (input == "quit") {
            stop = true;
            search_stop = true;
        }
        // stops the running prosses
        else if (input == "stop") {
            search_stop = true;
        }
        else {
            // if the last instruction is finished
            if (fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                search_stop = false;
                // start a new instruction
                fut = std::async(std::launch::async, protocol, std::ref(pos), input, std::ref(search_stop), std::ref(next_protocol_str), std::ref(next_protocol_mtx));
            }
            else {
                next_protocol_mtx.lock();
                if (next_protocol_str == "")
                    next_protocol_str = input;
                else
                    std::cout << "ERROR: There is still a running process and one waiting";
                next_protocol_mtx.unlock();
            }
        }
    }
    return 0;
}

