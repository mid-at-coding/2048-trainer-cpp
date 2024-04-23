#include "move.hpp"

bool move(Board& board, std::vector<int> positions){
	int curr = 0;
	int j = 0;
	bool moved = 0;
	int a = board[positions[0]];
	for(int i = 0; i < positions.size(); i++){
		int b = board[positions[i]];
		if((a == b && a > 0) || (a == 0 && b > 0)){
			moved = true;
			break;
		}
		a = b;
	}
	for(int i = 0; i < positions.size(); i++){
		int now = board[positions[i]];
		if(!now)
			continue;
		else if(!curr){
			curr = now;
			continue;
		}
		int p = positions[j];
		board[p] = curr;
		if(curr==now){
			board[p] += 1;
			curr=0;
		}
		else
			curr=now;
		j++;
	}
	if(curr){
		board[positions[j]] = curr;
		j++;
	}
	while(j < positions.size()){
		board[positions[j]] = 0;
		j++;
	}

	return moved;
}
bool pair(Board& board, int a, int b){
	int c = board[a];
	int d = board[b];
	if (c + d == 0)
		return false;

	if(c == 0 || c == d){ // ??
		board[a] += d;
		board[b] = 0;
		return true;
	}
	return false;
}
