#ifndef SOLVE_H
#define SOLVE_H
#include "board.hpp"
#include <map>
#include <vector>
struct boardProb{
	Board b;
	double p;
};
std::vector<Board> readBoards(int sum);
double evalprob(Board&,const std::map<Board,double>&,const std::map<Board,double>&);
void solve(int);
#endif
