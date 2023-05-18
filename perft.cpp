#include "types.h"
#include "movemaker.h"
#include "evaluation.h"


uint64_t perft (position &pos, int depth)
{
    if (depth == 0)
        return 1;
    uint64_t nodes = 0;
    std::array<int, 7> mv = moves(pos);
    for (int i = 0; i < 7; i++) {
        if (mv[i] == -1) {
            continue ;
        }
        do_move(pos, mv[i]);
        if (is_won(pos) == false)
            nodes += perft(pos, depth - 1);
        else {
            nodes += 1;
        }
        undo_move(pos, mv[i]);
    }
    if (nodes == 0)
        return 1;
    return nodes;
}
