#ifndef SOLVE_H
#define SOLVE_H
#include "board.hpp"
#include <map>
struct boardProb{
	board b;
	double p;
};
double evalprob(board&,const std::map<board,double>&,const std::map<board,double>&);
void solve(int);
#endif
