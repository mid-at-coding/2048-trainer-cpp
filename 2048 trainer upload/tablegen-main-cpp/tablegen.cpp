#include "board.hpp"
#include "error.hpp"
#include "config.hpp"
#include "seqlistfromposition.hpp"
#include "solve.hpp"
#include "formatting.hpp"

int main(int argc, char** argv){
	readConfig();
	Logger logger;
	Logger::outputThreshold = Logger::DEBUG;
	char input;
	logger.log(colorString("Welcome to CattoDoATable(s) v0.1", HSV{125, 60, 85}), Logger::INFO);
	do{
		if(argc > 1){ // for profiling
			input = '1';
			break;
		}
		logger.log("Would you like to (1) generate positions (2) solve(not done) ,(3) verify boards, or (4) list boards?", Logger::INFO);
		input = getchar();
	} while(input != '1' && input != '2' && input != '3' && input != '4');

	if(input == '1'){
		logger.log("(Re)generating positions", Logger::INFO);
		std::vector<Board> boards;
		Board startingBoard;
		startingBoard.board = 0;
		startingBoard[2] = 1;
		startingBoard[11] = 3;
		startingBoard[10] = 2;
		startingBoard[9] = 2;
		startingBoard[8] = 1;
		Logger::outputThreshold = Logger::DEBUG;
		gen_positions(startingBoard, getSum(startingBoard));
		exit(0);
	}
	if(input == '2'){
		logger.log("Solving", Logger::INFO);
		for(int i = std::stoi(config["s0"]); i > std::stoi(config["s1"]); i -= 2){
			solve(i);
		}
		exit(0);
	}
	if(input == '3'){
		std::string inp;
		int startingSum;
		int endSum;
		bool dumbass = false;
		getline(std::cin, inp);
		do{
			try{
				dumbass = false;
				logger.log("Enter starting sum to verify", Logger::INFO);
				getline(std::cin, inp);
				startingSum = std::stoi(inp);
			}
			catch(std::invalid_argument){
				dumbass = true;
			}
		}while(dumbass);
		dumbass = false;
		do{
			try{
				dumbass = false;
				logger.log("Enter ending sum to verify", Logger::INFO);
				getline(std::cin, inp);
				endSum = std::stoi(inp);
			}
			catch(std::invalid_argument){
				dumbass = true;
			}
		}while(dumbass);
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
		std::string inp;
		int s;
		bool dumbass = false;
		getline(std::cin, inp);
		do{
			try{
				dumbass = false;
				logger.log("Enter a sum to read", Logger::INFO);
				getline(std::cin, inp);
				s = std::stoi(inp);
			}
			catch(std::invalid_argument){
				dumbass = true;
			}
		}while(dumbass);
		auto boards = readBoards(s);
		for(int i = 0; i < boards.size(); i++){
			outputBoard(boards[i]);
		}
	}
}
