#include "move.hpp"

bool move(board& boar, std::vector<int> positions){
	int curr = 0;
	int j = 0;
	bool moved = 0;
	int a = boar.val[positions[0]];
	for(int i = 0; i < positions.size(); i++){
		int b = boar.val[positions[i]];
		if((a == b && a > 0) || (a == 0 && b > 0)){
			moved = true;
			break;
		}
		a = b;
	}
	for(int i = 0; i < positions.size(); i++){
		int now = boar.val[positions[i]];
		if(!now)
			continue;
		else if(!curr){
			curr = now;
			continue;
		}
		int p = positions[j];
		boar.val[p] = curr;
		if(curr==now){
			boar.val[p]<<=1;
			curr=0;
		}
		else
			curr=now;
		j++;
	}
	if(curr){
		boar.val[positions[j]] = curr;
		j++;
	}
	while(j < positions.size()){
		boar.val[positions[j]] = 0;
		j++;
	}

	return moved;
}
bool pair(board& boar, int a, int b){
	int c = boar.val[a];
	int d = boar.val[b];
	if (c + d == 0)
		return false;

	if(c == 0 || c == d){ // ??
		boar.val[a] += d;
		boar.val[b] = 0;
		return true;
	}
	return false;
}
