#ifndef BOARD_H
#define BOARD_H
#include <string>

struct board{
	int val[16] = {
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		0,0,0,0
	};
	int v;
	void regen();
};

#endif
