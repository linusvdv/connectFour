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
        TT_data.TT_hit    = true;
        TT_data.depth     = TT[hash].depth;
        TT_data.wdlm.win  = TT[hash].wdlm.win;
        TT_data.wdlm.draw = TT[hash].wdlm.draw;
        TT_data.wdlm.loss = TT[hash].wdlm.loss;
        TT_data.wdlm.mate = TT[hash].wdlm.mate;
        TT_data.mv        = TT[hash].mv;
        return TT_data;
    }
    TT_data.TT_hit = false;
    return TT_data;
}


void TT_set (position pos, int depth, wdlm_struct wdlm_value, int mv)
{
    unsigned int hash = TT_hash(pos);
    if (TT[hash].depth < depth + 4) {
        TT[hash].board     = pos.board;
        TT[hash].red       = pos.red;
        TT[hash].depth     = depth;
        TT[hash].wdlm.win  = wdlm_value.win;
        TT[hash].wdlm.draw = wdlm_value.draw;
        TT[hash].wdlm.loss = wdlm_value.loss;
        TT[hash].wdlm.mate = wdlm_value.mate;
        TT[hash].mv        = mv;
    }
}


void TT_clear ()
{
    for (int i = 0; i < int(TT_size); i++) {
        TT[i].board     = 0;
        TT[i].red       = 0;
        TT[i].depth     = 0;
        TT[i].wdlm.win  = 0;
        TT[i].wdlm.draw = 0;
        TT[i].wdlm.loss = 0;
        TT[i].wdlm.mate = 0;
        TT[i].mv        = 0;
    }
}
