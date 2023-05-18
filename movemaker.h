#include <array>

#include "types.h"


std::array<int, 7> moves (position pos);


void do_count (position &pos, int mv, int c);


void do_move (position &pos, int mv);


void undo_move (position &pos, int mv);
