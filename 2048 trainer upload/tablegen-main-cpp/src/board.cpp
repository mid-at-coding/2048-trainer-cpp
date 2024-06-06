#include "../include/board.hpp"
#include <cstdint>

Tile::Tile(Board* board, uint64_t off) {
	b = board;
	offset = off;
	val = GET_TILE((board->board), off);
}
Tile& Tile::operator=(const uint64_t& v){
	SET_TILE(((*b).board), (uint64_t)offset, (uint64_t)v);
	val = (v << OFFSET(15));
	return *this;
}
Tile& Tile::operator+=(const uint64_t& v){
	(*this) = (int)(*this) + v;
	return (*this);
}
Tile::operator uint8_t(){
	return (val & 0x0f);
}   
Tile Board::operator[](const int& x){
	return Tile(this, x);
}
bool Board::operator<(const Board& r) const{
	return board > r.board;
}
bool Board::operator>(const Board& r) const{
	return board > r.board;
}
bool Board::operator==(const Board& r) const{
	return board == r.board;
}

Board::Board(){
	board = 0;
}
