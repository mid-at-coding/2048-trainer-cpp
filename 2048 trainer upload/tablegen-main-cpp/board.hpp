#ifndef BOARD_H
#define BOARD_H
#include <cstdint>
#include <stdint.h>
#define MASK (0xf000000000000000)
#define OFFSET(x) (x * 4)
#define GET_TILE(b, x) (uint8_t)(((b << OFFSET(x)) & MASK) >> OFFSET(15))
#define SET_TILE(b, x, v) ( b = (~(~b | (MASK >> OFFSET(x))) | ((MASK & ((uint64_t)v << OFFSET(15))) >> OFFSET(x)) ))
#define SET_TILE2(b, x, v) ( b = (b & (UINT64_MAX - (uint64_t)(15 << OFFSET(x)))^((uint64_t)v << OFFSET(x))) )

class Board;

class Tile{
	Board* b;
	char offset : 4;
	char val : 4;
public:
	Tile(Board* board, uint64_t offset);
	Tile& operator=(const uint64_t& v);
	Tile& operator+=(const uint64_t& v);
	operator uint8_t();
};

class Board{
public:
	uint64_t board;
	Board();
	Tile operator[](const int&);
	bool operator<(const Board& r) const;
	bool operator>(const Board& r) const;
	bool operator==(const Board& r) const;
};

#endif
