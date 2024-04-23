#include "satisfied.hpp"

// dpdf win cond
bool satisfied(Board boar){
	if(boar[9] == 10)
		return true;

	return false;
}
