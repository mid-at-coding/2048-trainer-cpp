#ifndef MAPS_H
#define MAPS_H
#include <string>
#include "board.hpp"
// str2board wasn't in this file originally, but i feel it fits here better
// spam[maps[i]] = i.
// spam[i] where i is some alpha not => the numerical representation of that number
// linear[i] where i is some alpha not => the log of the numerical representation of that number
// maps[i] where i is some number    => the alpha not.representation of that number
// strToBoard(str)					 => board
constexpr int SPAM(char);
constexpr char LINEAR(int);
constexpr char MAP(int);
Board strToBoard(std::string);

#endif
