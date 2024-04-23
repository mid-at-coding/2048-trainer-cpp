#ifndef MOVE_H
#define MOVE_H
#include "board.hpp"
#include <vector>

// this function is basically 1:1 transpiled; i have no clue how or if it works
bool move(Board&, const std::vector<int>); // TODO maybe constexpr?
bool pair(Board&, int, int);

#endif
