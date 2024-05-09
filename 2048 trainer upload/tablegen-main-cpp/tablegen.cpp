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

int main(int argc, char** argv){
	readConfig();
	Logger logger;
	if(argc > 1){
		bool w = false;
		bool a = false;
		int sum = std::stoi(argv[1]);
		if(argc > 2){
			for(int i = 0; i < argc; i++){
				if(argv[i][0] == 'w'){
					w = true;
					Logger::outputThreshold = Logger::WARN;
				}
				if(argv[i][0] == 'a')
					a = true;
			}
		}
		do {
			auto boards = readBoards(sum);
			for(int i = 0; i < boards.size(); i++){
				logger.log("Sum: " + std::to_string(getSum(boards[i])), Logger::INFO);
				if(getSum(boards[i]) != sum){
					logger.log("Sums inequal; actual:" + std::to_string(getSum(boards[i])) + " supposed: " + std::to_string(sum), Logger::WARN);
					outputBoard(boards[i]);
					continue;
				}
				if(!w)
					outputBoard(boards[i]);
			}
			sum += 2;
		} while(a);
		return 0;
	}
	std::vector<Board> boards;
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
	Logger::outputThreshold = Logger::DEBUG;
	gen_positions(startingBoard, 518);
	logger.log("ALL DONE!!", Logger::INFO);
//	for(int i = std::stoi(config["s0"]); i > std::stoi(config["s1"]); i -= 2){
//		solve(i);
//	}
}
