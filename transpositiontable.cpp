#include "types.h"
#include <vector>


std::vector<TranspositionTable> TT(TT_size);


unsigned int TT_hash (position pos)
{
    unsigned int hash = ((uint128_t) pos.board << 64 | pos.color) % TT_size;
    return hash; 
}


TT_result TT_get (position pos)
{
    TT_result TT_data = {};
    unsigned int hash = TT_hash(pos);
    if (TT[hash].board == pos.board &&
        TT[hash].red   == pos.red) {
        TT_data.TT_hit = true;
        TT_data.depth = TT[hash].depth;
        TT_data.value = TT[hash].value;
        TT_data.mv    = TT[hash].mv;
        return TT_data;
    }
    TT_data.TT_hit = false;
    return TT_data;
}


void TT_set (position pos, int depth, int value, int mv)
{
    unsigned int hash = TT_hash(pos);
    if (TT[hash].depth < depth + 4) {
        TT[hash].board = pos.board;
        TT[hash].red = pos.red;
        TT[hash].depth = depth;
        TT[hash].value = value;
        TT[hash].mv = mv;
    }
}
