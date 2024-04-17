#include "satisfied.hpp"

// dpdf win cond
bool satisfied(board boar){
	if(boar.val[9] == 1024)
		return true;

	return false;
}
