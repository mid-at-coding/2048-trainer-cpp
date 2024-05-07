#include "movedir.hpp"
#include <vector>
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
