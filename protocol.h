#include <string>
#include <atomic>
#include <mutex>
#include <queue>
#include <condition_variable>

#include "types.h"


void clear_board (position &pos);


void protocol (position &pos, std::string input, std::atomic<bool>& search_stop);


void protocol_reader (std::queue<std::string>& protocol_queue, std::mutex& protocol_queue_mutex, std::condition_variable& protocol_cv, std::atomic<bool>& pending_task, std::atomic<bool>& search_stop, std::atomic<bool>& global_stop);
