#ifndef MAPS_H
#define MAPS_H
#include <string>
#include "board.hpp"
// str2board wasn't in this file originally, but i feel it fits here better
// spam[maps[i]] = i
// spam[i] where i is some alpha not    => the numerical representation of that number
// linear[i] where i is some power      => the numerical representation of that number
// common[i] where i is some power      => the numerical representation of that number, with 1024 and above replaced with 1k, 2k, so on
// map[i] where i is some number        => the alpha not.representation of that number
// strToBoard(str) where str is a board => board
int SPAM(char x);
int LINEAR(int x);
std::string COMMON(int x);
char MAP(int x);
Board strToBoard(std::string);

#endif
