#include "board.hpp"
#include <cmath>
void board::regen(){
	v = 0;
	for(int i = 0; i < 16; i++)
		v += std::pow(2,i) * val[i];
}
bool operator<(const board a, const board b){
	return a.v < b.v;
}
bool operator>(const board a, const board b){
	return a.v < b.v;
}
bool operator==(const board a, const board b){
	return a.v == b.v;
}
