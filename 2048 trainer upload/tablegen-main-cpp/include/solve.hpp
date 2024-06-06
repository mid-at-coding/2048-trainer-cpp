#ifndef SOLVE_H
#define SOLVE_H
#include "board.hpp"
#include <vector>
#include "BS_thread_pool.hpp"
#include "unordered_dense/include/ankerl/unordered_dense.h"
enum move{
	left = 0b1101,
	right = 0b1110,
	up = 0b1111,
	down = 0b1100
};
struct packed{
	double prob;
	enum move m;
	packed(){
		prob = 0;
		m = left;
	}
	std::string getString(){
		switch (m){
			case left:
				return "Left";
			case right:
				return "Right";
			case up:
				return "Up";
			case down:
				return "Down";
		}
	}
};
struct boardProb{
	Board b;
	packed p;
};
std::vector<Board> readBoards(int sum);
std::vector<boardProb> readTables(int sum);
double evalprob(Board& moved, const ankerl::unordered_dense::map<uint64_t, double>& twoSpawnProbs, const ankerl::unordered_dense::map<uint64_t, double>& fourSpawnProbs);
void solve(int, BS::thread_pool& q);
packed pack(enum move m, double prob);
boardProb unpack(std::vector<packed>& data, Board root);
#endif
