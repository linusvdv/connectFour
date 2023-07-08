#include <algorithm>
#include <condition_variable>
#include <cstdint>
#include <iostream>
#include <bit>
#include <array>
#include <bitset>
#include <iterator>
#include <ostream>
#include <queue>
#include <thread>
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

    // stops the search and globaly
    std::atomic<bool> search_stop = false;
    std::atomic<bool> global_stop = false;

    // protocol queue
    std::queue<std::string> protocol_queue;
    std::mutex protocol_queue_mutex;
    std::condition_variable protocol_cv;
    std::atomic<bool> pending_task = false;

    // start the protocol reader
    std::future<void> protocol_futur =
        std::async(
            std::launch::async,
            protocol_reader,
            std::ref(protocol_queue),
            std::ref(protocol_queue_mutex),
            std::ref(protocol_cv),
            std::ref(pending_task),
            std::ref(search_stop),
            std::ref(global_stop)
        );

    std::cout << "Connect Four by linusvdv" << std::endl;

    // starts the protocol if an item is in the queue
    while (true) {
        // wait until one element is in the queue
        {
            std::unique_lock lk(protocol_queue_mutex);
            protocol_cv.wait(lk, [&pending_task, &search_stop]{return pending_task || search_stop;});
        }

        // stops the program
        if (global_stop)
            break;

        // takes the first element of the queue and deletes it
        protocol_queue_mutex.lock();
        std::string protocol_input = protocol_queue.front();
        protocol_queue.pop();
        if (protocol_queue.size() == 0) {
            pending_task = false;
        }
        protocol_queue_mutex.unlock();

        // calling the protocol
        protocol(pos, protocol_input, search_stop);

        // stops the program
        if (global_stop)
            break;
        else if (search_stop)
            search_stop = false;
    }
}

