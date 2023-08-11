#include <atomic>


#include "types.h"


search_result alphabeta (position pos, int depth, wdlm_struct alpha, wdlm_struct beta, std::atomic<bool>& search_stop);
