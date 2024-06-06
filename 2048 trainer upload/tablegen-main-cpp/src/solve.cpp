#include "../include/solve.hpp"
#include "../include/board.hpp"
#include "../include/config.hpp"
#include "../include/maps.hpp"
#include "../include/error.hpp"
#include "../include/movedir.hpp"
#include "../include/satisfied.hpp"
#include "../include/BS_thread_pool.hpp"
#include <fstream>
#include <vector>
#include <string>
struct node{
	Board root;
	packed moves[4];
	node(){
		root.board = 0;
		for(int i = 0; i < 4; i++)
			moves[i] = packed();
	}
};
packed pack(enum move m, double prob){
	packed ret;
	ret.prob = prob;
	ret.m = m;
	return ret;
}

boardProb unpack(node data){
	boardProb ret;
	ret.b = data.root;
	double max = 0;
	packed curr;

	// find the highest probability
	for(int i = 0; i < 4; i++){
		curr = data.moves[i];
		if(curr.prob > max){
			max = curr.prob;
			ret.p.m = data.moves[i].m;
		}
	}
	ret.p.prob = max;
	return ret;
}

double evalprob(Board& moved, const ankerl::unordered_dense::map<uint64_t, double>& twoSpawnProbs, const ankerl::unordered_dense::map<uint64_t, double>& fourSpawnProbs){
	// expectimax
	static int spaces = std::stoi(config["spaces"]);
	int emptytiles = 0;
	double prob2 = 0;
	double prob4 = 0;
	if(satisfied(moved))
		return 1;
	for(int i = 0; i < spaces; i++){ 			// loop through all tiles
		if(moved[i]) 					// if there's a tile, skip this spot
			continue;
		emptytiles++;    					// increment number of empty tiles
		moved[i] = 1;					// change current empty tile to a two
		if(twoSpawnProbs.count(moved.board))
			prob2 += twoSpawnProbs.at(moved.board);	// add probability given two spawn to prob2
		moved[i] = 2;					// change current empty tile to a four
		if(fourSpawnProbs.count(moved.board))
			prob4 += fourSpawnProbs.at(moved.board);	// add probability given four spawn to prob4
		moved[i] = 0;					// reset tile to empty	
	}
	double ret = ((double)(0.9f * prob2) + (double)(0.1f * prob4)) / ((double)emptytiles);
	return ret; // average over amount of empty tiles and also spawn rate
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

std::vector<boardProb> readTables(int sum){
	Logger logger;
	node temp;
	Board root;
	std::ifstream file;
	std::streampos fileEnd;
	std::string fileName = "./" + config["tablesdir"] + "/" + std::to_string(sum) + ".tables";
	std::vector<boardProb> ret;

	file.open(fileName, std::ios::binary | std::ios::ate);
	if(!file.is_open()){
		logger.log("Could not open file: " + fileName, Logger::NONFATAL);
		return std::vector<boardProb>({});
	}
	fileEnd = file.tellg();
	file.seekg(0);
	for(int i = 0; i <= fileEnd; i+= (sizeof(node))){
		file.read(reinterpret_cast<char*>(&temp), sizeof(node));
		ret.push_back(unpack(temp)); // push back the unpacked node
	}
	return ret;
}

static void writeTable(int sum, std::vector<node> data){
	Logger logger;
	std::ofstream file;
	std::string fileName = "./" + config["tablesdir"] + "/" + std::to_string(sum) + ".tables";
	file.open(fileName);
	if(!file.is_open()){
		logger.log("Could not open file for writing: " + fileName, Logger::FATAL);
		exit(1);
	}
	for(int i = 0; i < data.size(); i++){
		file.write(reinterpret_cast<char*>(&data[i].root), sizeof(Board));
		file.write(reinterpret_cast<char*>(&data[i].moves[0]), sizeof(packed));
		file.write(reinterpret_cast<char*>(&data[i].moves[1]), sizeof(packed));
		file.write(reinterpret_cast<char*>(&data[i].moves[2]), sizeof(packed));
		file.write(reinterpret_cast<char*>(&data[i].moves[3]), sizeof(packed));
	}
}

void solve(int sum, BS::thread_pool &q){
	Logger logger;
	std::vector<boardProb> tempData; 
	std::vector<node> finalData;
	Board curr;
	int twoSum  = sum+2; 					// sum of tiles if a two spawns
	int fourSum = sum+4; 					// sum of tiles if a four spawns
	std::vector<Board> rawData; 		    // get all positions that have the same sum of tiles
	rawData = readBoards(sum);  				
	std::vector<Board> boards = rawData;
	static bool cached = false;
	static ankerl::unordered_dense::map<uint64_t, double> twoSpawnCache;  // if s+2 is calculated before s, s+4 will be s+2+2
	ankerl::unordered_dense::map<uint64_t, double> twoSpawnProbs;  // probabilities for boards with sum+2 tile sum
	ankerl::unordered_dense::map<uint64_t, double> fourSpawnProbs; // probabilities for boards with sum+4 tile sum

	if(cached){
		fourSpawnProbs = twoSpawnCache;
	}
	else{
		logger.log("Processing " + std::to_string(fourSum) + ".tables...", Logger::INFO);
		tempData	 = readTables(fourSum);	             		  // read tables with 4 spawns
		for(int i = 0; i < tempData.size(); i++){
			fourSpawnProbs[tempData[i].b.board] = tempData[i].p.prob;  // put these results into fourSpawnProbs
		}
		cached = true;
	}
	logger.log("Processing " + std::to_string(twoSum) + ".tables...", Logger::INFO);
	tempData  = readTables(twoSum);							 // read tables with 2 spawns
	for(int i = 0; i < tempData.size(); i++){
		twoSpawnProbs[tempData[i].b.board] = tempData[i].p.prob;  // put these results into twoSpawnProbs
	}
	twoSpawnCache = twoSpawnProbs; // cache for s-2 case
	logger.log("Solving s=" + std::to_string(sum), Logger::INFO);
	for(int i = 0; i < rawData.size(); i++){
		curr = rawData[i];
		finalData.push_back(node());
		finalData[i].root = curr;
		if(moveleft(curr)){
			finalData[i].moves[0] = (pack(left, evalprob(curr, twoSpawnProbs, fourSpawnProbs)));
			curr = rawData[i];
		}
		if(moveright(curr)){
			finalData[i].moves[1] = (pack(right, evalprob(curr, twoSpawnProbs, fourSpawnProbs)));
			curr = rawData[i];
		}
		if(moveup(curr)){
			finalData[i].moves[2] = (pack(up, evalprob(curr, twoSpawnProbs, fourSpawnProbs)));
			curr = rawData[i];
		}
		if(movedown(curr)){
			finalData[i].moves[3] = (pack(down, evalprob(curr, twoSpawnProbs, fourSpawnProbs)));
			curr = rawData[i];
		}
	}
	writeTable(sum, finalData); 
}
