#include <string>
#include <atomic>
#include <mutex>

#include "types.h"


void clear_board (position &pos);


void protocol (position &pos, std::string input, std::atomic<bool>& search_stop, std::string& next_protocol_str, std::mutex& next_protocol_mtx);
