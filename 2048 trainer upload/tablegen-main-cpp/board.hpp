#ifndef BOARD_H
#define BOARD_H
#include <stdint.h>
#define MASK (0xf000000000000000)
#define OFFSET(x) (x * 4)
#define GET_TILE(b, x) ((b << OFFSET(x)) & MASK)
#define SET_TILE(b, x, v) ( b = (~(~b | (MASK >> OFFSET(x))) | ((MASK & ((uint64_t)v << OFFSET(15))) >> OFFSET(x)) ))
#define SET_TILE2(b, x, v) ( b = (b & (UINT64_MAX - (uint64_t)(15 << OFFSET(x)))^((uint64_t)v << OFFSET(x))) )

class Board;

class Tile{
	Board* b;
	char data : 8;
public:
	Tile(Board* board, int offset);
	Tile& operator=(const int& v);
	Tile& operator+=(const int& v);
	operator int();
};

class Board{
public:
	unsigned long long board : 64;
	Tile operator[](const int&);
	bool operator<(const Board& r) const;
	bool operator>(const Board& r) const;
	bool operator==(const Board& r) const;
};

#endif
