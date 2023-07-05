#include <atomic>


#include "types.h"


std::tuple<int, int> alphabeta (position pos, int depth, int alpha, int beta, std::atomic<bool>& search_stop);
