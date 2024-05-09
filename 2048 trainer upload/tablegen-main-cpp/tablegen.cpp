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
	bool w = false;
	bool a = false;
	bool g = false;
	Logger::outputThreshold = Logger::DEBUG;
	logger.log("Number of args: " + std::to_string(argc), Logger::DEBUG);
	if(argc > 1){
		logger.log("Parsing args", Logger::DEBUG);
		for(int i = 0; i < argc; i++){
			if(argv[i][0] == 'w'){
				w = true;
				Logger::outputThreshold = Logger::WARN;
			}
			if(argv[i][0] == 'a')
				a = true;
			if(argv[i][0] == 'g')
				g = true;
		}
		if(w || a){
			int sum = std::stoi(argv[1]);
			do {
				auto boards = readBoards(sum);
				for(int i = 0; i < boards.size(); i++){
					logger.log("Sum: " + std::to_string(getSum(boards[i])), Logger::INFO);
					if(getSum(boards[i]) != sum){
						logger.log("Sums inequal; actual:" + std::to_string(getSum(boards[i])) + " supposed: " + std::to_string(sum), Logger::WARN);
						outputBoard(boards[i]);
						continue;
					}
					if(satisfied(boards[i])){
						logger.log("Board is a winstate:", Logger::WARN);
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
	}
	if(g){
		logger.log("(Re)generating positions", Logger::INFO);
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
	}
	for(int i = std::stoi(config["s0"]); i > std::stoi(config["s1"]); i -= 2){
		solve(i);
	}
}
