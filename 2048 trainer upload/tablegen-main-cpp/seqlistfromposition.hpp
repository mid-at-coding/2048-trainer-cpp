#ifndef SEQ_LIST_POS_H
#define SEQ_LIST_POS_H
#include "board.hpp"
#define BS_THREAD_POOL_ENABLE_PAUSE
#include "BS_thread_pool.hpp"
#include "parallel_hashmap/phmap.h"
#define MAX_MEM_GB 5

void outputBoard(Board&);
int getSum(Board&);
std::shared_ptr<std::vector<Board>> genSpawns(Board b, int n);

void processBoard(BS::thread_pool& q, std::shared_ptr<phmap::parallel_flat_hash_map<uint64_t, bool>> visited);

void gen_positions(Board& b, int sum);

void gen_layer(Board& b);

#endif
