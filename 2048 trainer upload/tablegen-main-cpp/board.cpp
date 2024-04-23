#include "board.hpp"

Tile::Tile(Board* board, int offset) {
	b = board;
	data = (MASK & offset);
	data = (data & (GET_TILE((*b).board, offset) >> 4));
}
Tile& Tile::operator=(const int& v){
	SET_TILE((*b).board, (MASK & data), ((MASK >> 4)  & data ));
	data = MASK & data; // clear second four bits
	data = data | (GET_TILE((*b).board, (MASK & data)) >> 4); // set value
	return *this;
}
Tile& Tile::operator+=(const int& v){
	(*this) = (int)(*this) + v;
	return (*this);
}
Tile::operator int(){
	return ((MASK >> 4) & data);
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
