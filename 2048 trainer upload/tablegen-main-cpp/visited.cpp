#include "visited.hpp"
#include <cstdlib>

void safe_map::clear(){
	for(uint64_t i = 0; i < SAFE_MAP_SIZE; i++)
		map[i] = false;
}
void safe_map::clear(uint64_t b){
	map[b % SAFE_MAP_SIZE] = false;
}
safe_map::safe_map(){
	map = (bool*)malloc(sizeof(bool) * SAFE_MAP_SIZE);
	clear();
}
bool safe_map::operator[](uint64_t b){
	if(map[b % SAFE_MAP_SIZE])
		return true;
	map[b % SAFE_MAP_SIZE] = true;
	return false;
}
