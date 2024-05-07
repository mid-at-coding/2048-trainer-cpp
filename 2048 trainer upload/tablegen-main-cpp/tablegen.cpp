#include "board.hpp"
#include "error.hpp"
#include "maps.hpp"
#include "config.hpp"
#include "move.hpp"
#include "movedir.hpp"
#include "satisfied.hpp"
#include "seqlistfromposition.hpp"
#include "solve.hpp"
#include <bitset>
#include <cstdint>
#include <iostream>
#include <iomanip>

int main(){
	readConfig();
	std::vector<Board> boards;
	Logger logger;
	Board startingBoard;
	startingBoard.board = 0;
	startingBoard[1] = 3;
	startingBoard[2] = 3;
	startingBoard[3] = 4;
	startingBoard[4] = 1;
	startingBoard[8] = 1;
	startingBoard[10] = 1;
	startingBoard[5] = 5;
	startingBoard[6] = 6;
	startingBoard[7] = 7;
	startingBoard[9] = 8;
	gen_positions(startingBoard, 518);
	logger.log("ALL DONE!!", Logger::INFO);
//	for(int i = std::stoi(config["s0"]); i > std::stoi(config["s1"]); i -= 2){
//		solve(i);
//	}
}
