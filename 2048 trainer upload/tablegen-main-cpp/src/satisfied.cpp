#include "../include/satisfied.hpp"
// dpdf win cond
#define DPDF
#ifdef DPDF
bool satisfied(Board boar){
	if(GET_TILE(boar.board,9) == 10)
		return true;

	return false;
} 
#endif
#ifdef TWELVE_SPACE
bool satisfied(Board b){
	if(GET_TILE(b.board,6) == 12)
		return true;
	if(GET_TILE(b.board,9) == 12)
		return true;

	return false;
} 
#endif
