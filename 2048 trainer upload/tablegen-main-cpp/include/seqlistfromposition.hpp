#ifndef SEQ_LIST_POS_H
#define SEQ_LIST_POS_H
#include "board.hpp"
#define BS_THREAD_POOL_ENABLE_PAUSE
#include <vector>
#include "unordered_dense/include/ankerl/unordered_dense.h"
#define MAX_MEM_GB 5

void outputBoard(Board&);
int getSum(Board&);
void genSpawns(const Board& b, int n, std::vector<Board>* ret);

void processBoard(std::shared_ptr<ankerl::unordered_dense::map<uint64_t, bool>> visited);

void gen_positions(Board& b, const int sum, const int cores);

void gen_layer(Board& b);

#endif
