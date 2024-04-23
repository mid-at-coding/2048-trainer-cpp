#ifndef SOLVE_H
#define SOLVE_H
#include "board.hpp"
#include <map>
struct boardProb{
	Board b;
	double p;
};
double evalprob(Board&,const std::map<Board,double>&,const std::map<Board,double>&);
void solve(int);
#endif
