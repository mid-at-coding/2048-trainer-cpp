#include "solve.hpp"
#include "board.hpp"
#include "config.hpp"
#include "maps.hpp"
#include "error.hpp"
#include "movedir.hpp"
#include "satisfied.hpp"
#include <fstream>
#include <vector>
#include <string>

double evalprob(board& moved, const std::map<board, double>& twoSpawnProbs, const std::map<board, double>& fourSpawnProbs){
	// expectimax
	int emptytiles = 0;
	double prob2 = 0;
	double prob4 = 0;
	for(int i = 0; i < 16; i++){ 			// loop through all tiles
		if(moved.val[i]) 					// if there's a tile, skip this spot
			continue;
		emptytiles++;    					// increment number of empty tiles
		moved.val[i] = 2;					// change current empty tile to a two
		prob2 += twoSpawnProbs.at(moved);	// add probability given two spawn to prob2
		moved.val[i] = 4;					// change current empty tile to a four
		prob2 += fourSpawnProbs.at(moved);	// add probability given four spawn to prob2
		moved.val[i] = 0;					// reset tile to empty	
	}
	return ((9*prob2+prob4)/10)/emptytiles; // average over amount of empty tiles and also spawn rate
}

static std::vector<board> processBoards(std::vector<std::string> rawData){
	std::vector<board> ret;
	for(int i = 0; i < rawData.size(); i++){
		ret.push_back(strToBoard(rawData[i]));
	}
	return ret;
}

static std::vector<boardProb> processBoardsWithProb(std::vector<std::string> rawData){
	std::vector<boardProb> ret;
	std::string board;
	std::string prob;
	bool set = false;
	for(int i = 0; i < rawData.size(); i++){
		board.clear();
		prob.clear();
		for(int j = 0; j < rawData[i].size(); j++){
			if(rawData[i][j] == ' '){ // if there's a space, switch to reading probability
				set = true;
				continue;
			}
			if(set) // read probability
				prob += rawData[i][j];
			else    // read boardstate
				board += rawData[i][j];
		}
		ret.push_back(boardProb{strToBoard(board), std::stof(prob.c_str())});
	}
	return ret;
}
static std::vector<std::string> readBoards(int sum){
	Logger logger;
	std::vector<std::string> ret;
	std::string temp;
	std::ifstream file;
	std::string fileName = "./" + config["positionsdir"] + "/" + std::to_string(sum);
	file.open(fileName);
	if(!file.is_open()){
		logger.log("Could not open file: " + fileName, Logger::FATAL);
		exit(1);
	}
	while(getline(file, temp))
		ret.push_back(temp);
	return ret;
}

static std::vector<std::string> readTables(int sum){
	Logger logger;
	std::vector<std::string> ret;
	std::string temp;
	std::ifstream file;
	std::string fileName = "./" + config["tablesdir"] + "/" + std::to_string(sum);
	file.open(fileName);
	if(!file.is_open()){
		logger.log("Could not open file: " + fileName, Logger::FATAL);
		exit(1);
	}
	while(getline(file, temp))
		ret.push_back(temp);
	return ret;
}

static void writeTable(int sum, std::string data){
	Logger logger;
	std::ofstream file;
	std::string fileName = "./" + config["tablesdir"] + "/" + std::to_string(sum) + ".txt";
	file.open(fileName);
	if(!file.is_open()){
		logger.log("Could not open file: " + fileName, Logger::FATAL);
		exit(1);
	}
	file << data;
}

void solve(int sum){
	board moved;
	board currentBoard;
	std::string finalData;
	Logger logger;
	std::vector<std::string> tempRawData; 
	std::vector<boardProb> readData;
	std::vector<std::string> probs;
	int twoSum  = sum+2; 					// sum of tiles if a two spawns
	int fourSum = sum+4; 					// sum of tiles if a four spawns
	std::vector<std::string> rawData; 		// get all positions that have the same sum of tiles
	rawData = readBoards(sum);  				
	std::vector<board> boards = processBoards(rawData);  	
	static bool cached = false;
	static std::map<board, double> twoSpawnCache;  // if s+2 is calculated before s, s+4 will be s+2+2
											// no need to recalculate s+4, because s+2+2 is there
	std::map<board, double> twoSpawnProbs;  // probabilities for boards with sum+2 tile sum
	std::map<board, double> fourSpawnProbs; // probabilities for boards with sum+4 tile sum
	if(cached){
		fourSpawnProbs = twoSpawnCache;
	}
	else{
		logger.log("Processing " + std::to_string(fourSum) + ".txt...", Logger::INFO);
		tempRawData	 = readTables(fourSum);	                  	// read boards with probability (assuming 4 spawn)
		readData = processBoardsWithProb(tempRawData);			// process boards
		for(int i = 0; i < readData.size(); i++){
			fourSpawnProbs[readData[i].b] = readData[i].p;  // put these results into fourSpawnProbs
		}
	}
	logger.log("Processing " + std::to_string(twoSum) + ".txt...", Logger::INFO);
	tempRawData  = readTables(twoSum);				// read boards with probability (assuming 2 spawn)
	readData = processBoardsWithProb(tempRawData);  // process boards
	for(int i = 0; i < readData.size(); i++){
		twoSpawnProbs[readData[i].b] = readData[i].p;  // put these results into twoSpawnProbs
	}
	twoSpawnCache = twoSpawnProbs; // cache for s-2 case
	logger.log("Solving s=" + std::to_string(sum), Logger::INFO);
	
	double bestProb = 0;
	double currentProb = 0;
	for(int i = 0; i < boards.size(); i++){
		// go through all the boards
		currentBoard = boards[i];
		if(satisfied(currentBoard)){
			// if this board is a winstate
			finalData = rawData[i] + "1\n";
			continue;
		}
		moved = currentBoard;// copy currentBoard into moved
		if(moveleft(moved)){ // if moved can move left
			probs.push_back("L"); // do shit
			currentProb = evalprob(moved,twoSpawnProbs,fourSpawnProbs);
			probs.push_back(std::to_string(currentProb));
			bestProb = std::max(currentProb, bestProb);
		} 
		moved = currentBoard;
		if(moveright(moved)){ // do it for the other directions
			probs.push_back("R"); 
			currentProb = evalprob(moved,twoSpawnProbs,fourSpawnProbs);
			probs.push_back(std::to_string(currentProb));
			bestProb = std::max(currentProb, bestProb);
		}
		moved = currentBoard;
		if(moveup(moved)){ 
			probs.push_back("U"); 
			currentProb = evalprob(moved,twoSpawnProbs,fourSpawnProbs);
			probs.push_back(std::to_string(currentProb));
			bestProb = std::max(currentProb, bestProb);
		}
		moved = currentBoard;
		if(movedown(moved)){ 
			probs.push_back("D"); 
			currentProb = evalprob(moved,twoSpawnProbs,fourSpawnProbs);
			probs.push_back(std::to_string(currentProb));
			bestProb = std::max(currentProb, bestProb);
		}
		for(int i = 0; i < probs.size(); i++){
			finalData += probs[i];
			if(i != probs.size() - 1)
				finalData += " ";
		}
		finalData += "\n";
	}
	writeTable(sum, finalData);
}
