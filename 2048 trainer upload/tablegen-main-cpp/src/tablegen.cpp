#define DPDF
#include "../include/board.hpp"
#include "../include/ui.hpp"
#include "../include/error.hpp"
#include "../include/config.hpp"
#include "../include/seqlistfromposition.hpp"
#include "../include/solve.hpp"
#include "../include/formatting.hpp"
#include "../include/BS_thread_pool.hpp"
#include <thread>

int main(int argc, char** argv){
	readConfig();
	Logger logger;
	Logger::outputThreshold = Logger::DEBUG;
	int input = 0;
	
	logger.log(colorString("Welcome to CattoDoATable(s) v0.1", HSV{125, 60, 85}), Logger::INFO);
	logger.log("Would you like to (1) generate positions (2) solve(not done), (3) verify boards, (4) list boards, or (5) list tables?", Logger::INFO);
	do{
		setInt(&input);
	}while(input == 0);

	if(input == 1){
		int cores = getNumberInRange(0, std::thread::hardware_concurrency(), "number of cores");
		logger.log("(Re)generating positions", Logger::INFO);
		std::vector<Board> boards;
		Board startingBoard;
#ifdef TWELVE_SPACE
		startingBoard.board = 0;
		startingBoard[2] = 1;
		startingBoard[11] = 3;
		startingBoard[10] = 2;
		startingBoard[9] = 2;
		startingBoard[8] = 1;
#endif
#ifdef DPDF
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
#endif
		gen_positions(startingBoard, getSum(startingBoard), cores);
		exit(0);
	}
	if(input == '2'){
		int cores = getNumberInRange(0, std::thread::hardware_concurrency(), "number of cores");
		auto q = BS::thread_pool(cores);
		logger.log("Solving", Logger::INFO);
		for(int i = std::stoi(config["s0"]); i > std::stoi(config["s1"]); i -= 2){
			solve(i, q);
		}
		exit(0);
	}
	if(input == '3'){
		int startingSum = getNumberInRange(0, std::stoi(config["s0"]), "starting sum");
		int endSum = getNumberInRange(0, std::stoi(config["s0"]), "ending sum");
		for(int s = startingSum; s < endSum; s += 2){
			auto boards = readBoards(s);
			logger.log("Sum: " + std::to_string(s), Logger::INFO);
			for(int i = 0; i < boards.size(); i++){
				if(getSum(boards[i]) != s){
					logger.log("Sums inequal( actual: " + std::to_string(getSum(boards[i])) + " supposed: " + std::to_string(s) + " )", Logger::WARN);
					outputBoard(boards[i]);
				}
			}
		}
	}
	if(input == '4'){
		int s = getNumberInRange(0, std::stoi(config["s0"]), "sum to read");
		auto boards = readBoards(s);
		for(int i = 0; i < boards.size(); i++){
			outputBoard(boards[i]);
		}
	}
	if(input == '5'){
		int s = getNumberInRange(0, std::stoi(config["s0"]), "sum to read");
		auto boards = readTables(s);
		for(int i = 0; i < boards.size(); i++){
			logger.log("Prob: " + std::to_string(boards[i].p.prob), Logger::INFO);
			outputBoard(boards[i].b);
		}
	}
}
