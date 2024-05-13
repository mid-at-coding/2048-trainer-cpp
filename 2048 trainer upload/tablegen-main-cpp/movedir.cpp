#include "movedir.hpp"
#include <vector>
#define DPDF
#ifdef DPDF
bool moveleft(Board& board){
	if(board[8]*board[9]*board[10]==0 || board[8]==board[9]){ return false; }
	return move(board,std::vector<int>({0,1,2,3}))|move(board,std::vector<int>({4,5,6,7}));
}
bool moveright(Board& board){
	return move(board, std::vector<int>({3,2,1,0})) || move(board, std::vector<int>({7,6,5,4})) || pair(board, 9, 8);
}
bool moveup(Board& board){
	if(board[1]*board[2]*board[3]*board[5]*board[6]*board[7]*board[9]==0||board[1]==board[5]||board[9]==board[5]||board[2]==board[6]||board[3]==board[7]){return 0;};
	return move(board,std::vector<int>({0,4,8,10}));
}
bool movedown(Board& board){
	return move(board,std::vector<int>({10,8,4,0}))|move(board,std::vector<int>({9,5,1}))|pair(board,6,2)|pair(board,7,3);
}; 
#endif
#ifdef TWELVE_SPACE
bool moveleft(Board& b){
	if(b[8]*b[9]*b[10]*b[11]==0||b[8]==b[9]||b[10]==b[11]){ return 0; };
	return move(b,std::vector<int>({0,1,2,3}))|move(b,std::vector<int>({4,5,6,7}));
};
bool moveright(Board& b){
	return move(b,std::vector<int>({3,2,1,0}))|move(b,std::vector<int>({7,6,5,4}))|pair(b,9,8)|pair(b,11,10);
};
bool moveup(Board& b){
	if(b[2]*b[3]*b[6]*b[7]==0||b[2]==b[6]||b[3]==b[7]){ return 0; };
	return move(b,std::vector<int>({0,4,8,10}))|move(b,std::vector<int>({1,5,9,11}));
};
bool movedown(Board& b){
	return move(b,std::vector<int>({10,8,4,0}))|move(b,std::vector<int>({11,9,5,1}))|pair(b,6,2)|pair(b,7,3);
};
#endif

// 0  1  2  3
// 4  5  6  7
// 8  9  x  x
// 10 11 x  x
