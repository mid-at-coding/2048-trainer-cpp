#include "movedir.hpp"
#include <vector>
bool moveleft(board boar){
	if(boar.val[8]*boar.val[9]*boar.val[10]==0 || boar.val[8]==boar.val[9]){ return false; }
	return move(boar,std::vector<int>({0,1,2,3}))|move(boar,std::vector<int>({4,5,6,7}));
}
bool moveright(board boar){
	return move(boar, std::vector<int>({3,2,1,0})) || move(boar, std::vector<int>({7,6,5,4})) || pair(boar, 9, 8);
}
bool moveup(board boar){
	if(boar.val[1]*boar.val[2]*boar.val[3]*boar.val[5]*boar.val[6]*boar.val[7]*boar.val[9]==0||boar.val[1]==boar.val[5]||boar.val[9]==boar.val[5]||boar.val[2]==boar.val[6]||boar.val[3]==boar.val[7]){return 0;};
	return move(boar,std::vector<int>({0,4,8,10}));
}
