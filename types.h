#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED


#include <array>
#include <cstdint>


typedef unsigned __int128 uint128_t;


struct position {
    // board layout:
    // 35 36 37 38 39 40 41
    // 28 29 30 31 32 33 34
    // 21 22 23 24 25 26 27
    // 14 15 16 17 18 19 20
    //  7  8  9 10 11 12 13
    //  0  1  2  3  4  5  6
    uint64_t board = 0;
    uint64_t red = 0;
    // -1 red
    //  0 yellow
    uint64_t color = -1;
    // count of pieces
    // outer array
    // - horrizontal
    // - vertcial
    // - diagonal right up/left down
    // - diagonal right down/left up
    // middle array
    // - 0th - red
    // - 1st - yellow
    // inner array
    // - 0th - 0 pieces
    // - 1st - 1 pieces
    // - 2nd - 2 pieces
    // - 3rd - 3 pieces
    // - 4th - 4 pieces
    std::array<std::array<std::array<uint64_t, 5>, 2>, 4> cross_count = {{
        {{{519454312335, 0, 0, 0, 0}, {519454312335, 0, 0, 0, 0}}},
        {{{2097151, 0, 0, 0, 0}, {2097151, 0, 0, 0, 0}}},
        {{{247695, 0, 0, 0, 0}, {247695, 0, 0, 0, 0}}},
        {{{1981560, 0, 0, 0, 0}, {1981560, 0, 0, 0, 0}}}
    }};
};


struct TranspositionTable {
    // needed to verifie the board
    uint64_t board;
    uint64_t red;
    // additional information
    int depth;
    int value;
    int mv;
};


struct TT_result {
    bool TT_hit;
    int depth;
    int value;
    int mv;
};


// this number has to be a prime number
const unsigned int TT_size = 10000019;


constexpr std::array<uint64_t, 7> columns = {
    0b000000100000010000001000000100000010000001 << 0,
    0b000000100000010000001000000100000010000001 << 1,
    0b000000100000010000001000000100000010000001 << 2,
    0b000000100000010000001000000100000010000001 << 3,
    0b000000100000010000001000000100000010000001 << 4,
    0b000000100000010000001000000100000010000001 << 5,
    0b000000100000010000001000000100000010000001 << 6
};


constexpr std::array<uint64_t, 4> cross_types = {
    (1LL << 21) | (1LL << 22) | (1LL << 23) | (1LL << 24),
    (1LL <<  3) | (1LL << 10) | (1LL << 17) | (1LL << 24),
    (1LL <<  0) | (1LL <<  8) | (1LL << 16) | (1LL << 24),
    (1LL <<  6) | (1LL << 12) | (1LL << 18) | (1LL << 24)
};


#endif
