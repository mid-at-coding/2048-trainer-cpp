#include "visited.hpp"
#include <cstdlib>
#include "xxhash/xxhash32.h"
#include "error.hpp"
#include "seqlistfromposition.hpp"

void safe_map::clear(){
	for(uint64_t i = 0; i < SAFE_MAP_SIZE; i++)
		map[i] = 0;
}
void safe_map::clear(const uint64_t& b){
	uint64_t bc = b;
	uint32_t index = XXHash32::hash(&bc, 8, bc); 
	map[index] = 0;
}
safe_map::safe_map(){
	map = (uint32_t*)malloc(sizeof(uint32_t) * (4294967295));
	clear();
}

bool safe_map::operator[](const uint64_t& b){
	if(map[b % SAFE_MAP_SIZE])
		return true;
	map[b % SAFE_MAP_SIZE] = true;
	return false;
}

bool safe_map::visit(const uint64_t& b){
	uint64_t bc = b;
	Board bo;
	bo.board = b;
	uint32_t index = XXHash32::hash(&bc, 8, bc); 
	uint32_t val = getSum(bo);
	Logger logger;
	if(!map[index]){ 
		map[index] = val;
		return false;
	}
	if(map[index] != val){ // ruh roh
		collisions++;
		return false;
	}
	return true;
}
