#include "solve.hpp"
#include "board.hpp"
#include "config.hpp"
#include "maps.hpp"
#include "error.hpp"
#include "movedir.hpp"
#include "satisfied.hpp"
#include "BS_thread_pool.hpp"
#include "parallel_hashmap/phmap.h"
#include "seqlistfromposition.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <bitset>

enum move{
	left,
	right,
	up,
	down
};

static BS::thread_pool q = BS::thread_pool(2);

static Board pack(enum move m, double prob){
	Board ret;
	uint64_t fixed_point_60 = prob * (std::pow(2, 61) - 1);
	std::cout << std::bitset<64>(fixed_point_60) << std::endl;
	if(m == left)
		ret[0] = 0b1000;
	if(m == right)
		ret[0] = 0b0100;
	if(m == up)
		ret[0] = 0b0010;
	if(m == down)
		ret[0] = 0b0001;
	return ret;
}

double evalprob(Board& moved, const phmap::parallel_flat_hash_map<uint64_t, double>& twoSpawnProbs, const phmap::parallel_flat_hash_map<uint64_t, double>& fourSpawnProbs){
	// expectimax
	int emptytiles = 0;
	double prob2 = 0;
	double prob4 = 0;
	for(int i = 0; i < 16; i++){ 			// loop through all tiles
		if(moved[i]) 					// if there's a tile, skip this spot
			continue;
		emptytiles++;    					// increment number of empty tiles
		moved[i] = 2;					// change current empty tile to a two
		if(twoSpawnProbs.count(moved.board))
			prob2 += twoSpawnProbs.at(moved.board);	// add probability given two spawn to prob2
		moved[i] = 4;					// change current empty tile to a four
		if(fourSpawnProbs.count(moved.board))
			prob4 += fourSpawnProbs.at(moved.board);	// add probability given four spawn to prob4
		moved[i] = 0;					// reset tile to empty	
	}
	return ((9*prob2+prob4)/10)/emptytiles; // average over amount of empty tiles and also spawn rate
}

static std::vector<Board> processBoards(std::vector<std::string> rawData){
	std::vector<Board> ret;
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
std::vector<Board> readBoards(int sum){
	Logger logger;
	std::vector<Board> ret;
	Board temp;
	std::ifstream file;
	std::streampos fileEnd;
	std::string fileName = "./" + config["positionsdir"] + "/" + std::to_string(sum) + ".tables";
	file.open(fileName, std::ios::binary | std::ios::ate);
	if(!file.is_open()){
		logger.log("Could not open file: " + fileName, Logger::FATAL);
		exit(1);
	}
	fileEnd = file.tellg();
	file.seekg(0);
	for(int i = 0; i < fileEnd; i+= sizeof(Board)){
		file.read(reinterpret_cast<char*>(&temp), sizeof(Board));
		ret.push_back(temp);
	}
	return ret;
}

static std::vector<boardProb> readTables(int sum){
	Logger logger;
	std::vector<boardProb> ret;
	Board temp;
	double tempProb;
	std::ifstream file;
	std::streampos fileEnd;
	std::string fileName = "./" + config["tablesdir"] + "/" + std::to_string(sum) + ".tables";
	file.open(fileName, std::ios::binary | std::ios::ate);
	if(!file.is_open()){
		logger.log("Could not open file: " + fileName, Logger::NONFATAL);
		return std::vector<boardProb>({});
	}
	fileEnd = file.tellg();
	file.seekg(0);
	for(int i = 0; i <= fileEnd; i+= (sizeof(Board) + sizeof(double))){
		file.read(reinterpret_cast<char*>(&temp), sizeof(Board));
		file.read(reinterpret_cast<char*>(&tempProb), sizeof(double));
		ret.push_back(boardProb{temp, tempProb});
	}
	return ret;
}

static void writeTable(int sum, std::vector<boardProb> data){
	Logger logger;
	std::ofstream file;
	std::string fileName = "./" + config["tablesdir"] + "/" + std::to_string(sum) + ".tables";
	file.open(fileName);
	if(!file.is_open()){
		logger.log("Could not open file for writing: " + fileName, Logger::FATAL);
		exit(1);
	}
	for(int i = 0; i < data.size(); i++){
		file.write(reinterpret_cast<char*>(&data[i].b.board), sizeof(uint64_t));
		file.write(reinterpret_cast<char*>(&data[i].p), sizeof(double));
	}
}

void solve(int sum){
	Logger logger;
	std::vector<boardProb> tempData; 
	std::vector<boardProb> consolidatedData;
	int twoSum  = sum+2; 					// sum of tiles if a two spawns
	int fourSum = sum+4; 					// sum of tiles if a four spawns
	std::vector<Board> rawData; 		    // get all positions that have the same sum of tiles
	rawData = readBoards(sum);  				
	std::vector<Board> boards = rawData;
	static bool cached = false;
	static phmap::parallel_flat_hash_map<uint64_t, double> twoSpawnCache;  // if s+2 is calculated before s, s+4 will be s+2+2
	phmap::parallel_flat_hash_map<uint64_t, double> twoSpawnProbs;  // probabilities for boards with sum+2 tile sum
	phmap::parallel_flat_hash_map<uint64_t, double> fourSpawnProbs; // probabilities for boards with sum+4 tile sum

	if(cached){
		fourSpawnProbs = twoSpawnCache;
	}
	else{
		logger.log("Processing " + std::to_string(fourSum) + ".tables...", Logger::INFO);
		tempData	 = readTables(fourSum);	             	// read boards with probability (assuming 4 spawn)
		for(int i = 0; i < tempData.size(); i++){
			fourSpawnProbs[tempData[i].b.board] = tempData[i].p;  // put these results into fourSpawnProbs
		}
	}
	logger.log("Processing " + std::to_string(twoSum) + ".tables...", Logger::INFO);
	tempData  = readTables(twoSum);						// read boards with probability (assuming 2 spawn)
	for(int i = 0; i < tempData.size(); i++){
		twoSpawnProbs[tempData[i].b.board] = tempData[i].p;  // put these results into twoSpawnProbs
	}
	twoSpawnCache = twoSpawnProbs; // cache for s-2 case
	logger.log("Solving s=" + std::to_string(sum), Logger::INFO);
	const int size = boards.size();
	auto data = q.submit_blocks(0, size, [&boards, &rawData, &twoSpawnProbs, &fourSpawnProbs](const int start, const int end){
		Board moved;
		Board currentBoard;
		boardProb bestProb;
		boardProb currentProb;
		Board emptyBoard;
		Logger logger;
		std::vector<boardProb> probs = {};
		std::vector<boardProb> finalData = {};
		for(int i = start; i < end; i++){
			emptyBoard.board = 0;
			bestProb.b.board = 0;
			currentProb.b.board = 0;
			bestProb.p = 0;
			currentProb.p = 0;
			// go through all the boards
			currentBoard = boards[i];
			if(satisfied(currentBoard)){
				// if this board is a winstate
				logger.log("Winstate", Logger::DEBUG);
				currentProb.b = rawData[i];
				currentProb.p = 1;
				finalData.push_back(currentProb);
				continue;
			}
			moved = currentBoard;// copy currentBoard into moved
			if(moveleft(moved)){ // if moved can move left
				currentProb.b[0] = 0b1000; // magic code
				currentProb.p = evalprob(moved,twoSpawnProbs,fourSpawnProbs);
				if(currentProb.p != 0){
					logger.log("Prob:" + std::to_string(currentProb.p), Logger::INFO);
					outputBoard(moved);
					pack(left, currentProb.p);
					exit(0);
				}
				probs.push_back(currentProb); // do shit
				bestProb.p = std::max(currentProb.p, bestProb.p);
			} 
			moved = currentBoard;
			if(moveright(moved)){ // do it for the other directions
				currentProb.b[0] = 0b0100;
				currentProb.p = evalprob(moved,twoSpawnProbs,fourSpawnProbs);
				probs.push_back(currentProb);
				bestProb.p = std::max(currentProb.p, bestProb.p);
			}
			moved = currentBoard;
			if(moveup(moved)){ 
				currentProb.b[0] = 0b0010;
				currentProb.p = evalprob(moved,twoSpawnProbs,fourSpawnProbs);
				probs.push_back(currentProb);
				bestProb.p = std::max(currentProb.p, bestProb.p);
			}
			moved = currentBoard;
			if(movedown(moved)){ 
				currentProb.b[0] = 0b0001;
				currentProb.p = evalprob(moved,twoSpawnProbs,fourSpawnProbs);
				probs.push_back(currentProb);
				bestProb.p = std::max(currentProb.p, bestProb.p);
			}
			for(int i = 0; i < probs.size(); i++){
				finalData.push_back(probs[i]);
			}
			currentProb.b[0] = 0b1111;
			finalData.push_back(currentProb);
		}
		return finalData;
	});
	data.wait();
	consolidatedData.clear();
	for(int i = 0; i < data.size(); i++){
		auto block = data[i].get();
		for(int j = 0; j < block.size(); j++)
			consolidatedData.push_back(block[j]);
	} 
	writeTable(sum, consolidatedData);
}
