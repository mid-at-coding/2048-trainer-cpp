#include "board.hpp"
#include "error.hpp"
#include "config.hpp"
#include "BS_thread_pool.hpp" // gh.com/bshoshany/thread_pool
#include <cstdint>
#include "movedir.hpp"
#include <fstream>
#include <memory>
#include "seqlistfromposition.hpp"
#include "parallel_hashmap/phmap.h"
#include "maps.hpp"
#include <iomanip>
#include <mutex>
#include "visited.hpp"
#include "satisfied.hpp"
using namespace std::chrono_literals;
struct boardSpawn{
	Board b;
	int spawn;
};
static int globalsum;
static std::vector<uint64_t> n;
static std::vector<uint64_t> n2;
static std::vector<uint64_t> n4;
static std::mutex writeData_mutex;
static safe_map<uint64_t>* visited;
static std::mutex pool_mutex;
static BS::thread_pool q = BS::thread_pool(1);
static void writeB(
#ifdef COPY_BOARDS
		std::vector<uint64_t> n ,
#endif
		bool setSum = false, int sumIn = 0){
	static long long unsigned boardNum = 0;
	static long long unsigned layerNum = 0;
	static int sum;
	static int lastLayerSize = 1;
	static int s0 = std::stoi(config["s0"]);
	static auto firstWrite = std::chrono::system_clock::now();
	static auto lastWrite = std::chrono::system_clock::now();
	Logger logger;
	std::ofstream file;
	std::string fileName;
	auto now = std::chrono::system_clock::now();

	layerNum++;
	if(setSum){
		sum = sumIn;
		globalsum = sum;
		return;
	}
	logger.log("Sum: " + std::to_string(sum), Logger::INFO);
	boardNum += n.size();
	logger.log("Boards processed:  " + std::to_string(boardNum), Logger::INFO);
	if(n.size() - lastLayerSize)
		logger.log("Layer multiplier:  " + std::to_string((double)n.size() / lastLayerSize), Logger::INFO);
	lastLayerSize = n.size();
	logger.log("Layer size:  " + std::to_string(n.size()), Logger::INFO);
	if((now - lastWrite) / 1s != 0)
		logger.log("Layers per second: " + std::to_string((double)1 / (double)((now - lastWrite) / 1000ms)), Logger::INFO);
	if((now - lastWrite) / 1s != 0)
		logger.log("Boards per second: " + std::to_string(n.size() / ((now - lastWrite) / 1s)), Logger::INFO);
	if((now - firstWrite) / 1s != 0)
		logger.log("Average boards per second: " + std::to_string(boardNum / ((now - firstWrite) / 1s)), Logger::INFO);
	if((now - firstWrite) / 1s != 0){
		logger.log("Average layers per second: " + std::to_string(((double)layerNum) / ((now - firstWrite) / 1s)), Logger::INFO);
		logger.log("Estimated time remaining: " + std::to_string((int)((s0 - globalsum) / (((double)layerNum) / ((now - firstWrite) / 1s)))) + " seconds", Logger::INFO);
	}

#ifndef DRY_RUN
	fileName = "./" + config["positionsdir"] + "/" + std::to_string(sum) + ".tables";
	file.open(fileName);
	if(!file.is_open()){
		logger.log("Could not open file: " + fileName, Logger::FATAL);
		exit(1);
	}
#endif
	for(int i = 0; i < n.size(); i++){
#ifndef DRY_RUN
		file.write(reinterpret_cast<char*>(&(n[i])), sizeof(uint64_t));
		visited->clear((uint64_t)n[i]);
#endif
	}
	sum += 2;
	lastWrite = std::chrono::system_clock::now();
}
static bool checkForBadBoard(Board b){
	Logger logger;
	if(getSum(b) != globalsum){
		logger.log("Found bad board:(" + std::to_string(getSum(b)) + ")", Logger::FATAL);
		outputBoard(b);
		return true;
	}
	return false;
}
static bool checkForBadBoard(Board b, int spawn){
	Logger logger;
	if(getSum(b) != globalsum + spawn){
		logger.log("Found bad board:(" + std::to_string(getSum(b)) + ")", Logger::FATAL);
		outputBoard(b);
		return true;
	}
	return false;
}
static void checkForBadBoards(std::vector<Board> b){
	Logger logger;
	for(int i = 0; i < n.size(); i++){
		if(getSum(b[i]) != globalsum){
			logger.log("Found bad root board:(" + std::to_string(getSum(b[i])) + ")", Logger::FATAL);
			outputBoard(b[i]);
			exit(1);
		}
	}
}
void outputBoard(Board& b){
	int pos = 0;
	for(int i = 0; i < 16; i++){
		if(i == 10 || i == 11 || i == 14 || i == 15)
			std::cout << std::setw(3) << ("x");
		else
			std::cout << std::setw(3) << (COMMON(b[pos++]));
		std::cout << " " ;
		if(!((i + 1) % 4))
			std::cout << std::endl;
	}
	std::cout << std::endl;
}
int getSum(Board& b){
	int s = 0;
	for(int i = 0; i < 16; i++){
		s += LINEAR(b[i]);
	}
	return s;
}

std::vector<Board>* genSpawns(const Board& b, int n){
	static int spaces = std::stoi(config["spaces"]);
	std::vector<Board>* ret = new std::vector<Board>;
	Logger logger;
	Board boardCopy = b;
	for(int i = 0; i < spaces; i++){
		if(GET_TILE(boardCopy.board, i) == 0){
			SET_TILE(boardCopy.board, i, n);
			ret->push_back(boardCopy);
			boardCopy = b;
		}
	}
	return ret;
}

static std::vector<Board>* genMoves(const Board& b){
	Logger logger;
	Board boardCopy = b;
	std::vector<Board>* ret = new std::vector<Board>;
	if(moveleft(boardCopy)){
		ret->push_back(boardCopy);
		boardCopy = b;
	}
	if(moveright(boardCopy)){
		ret->push_back(boardCopy);
		boardCopy = b;
	}
	if(moveup(boardCopy)){
		ret->push_back(boardCopy);
		boardCopy = b;
	}
	if(movedown(boardCopy)){
		ret->push_back(boardCopy);
		boardCopy = b;
	}
	return ret;
}

static bool addToWriteData(Board& b){
	if(!writeData_mutex.try_lock())
		return false;
	n.push_back(b.board);
	return true;
}

static void addToWriteData(std::vector<Board>& b){
	std::lock_guard lock(writeData_mutex);
	for(int i = 0; i < b.size(); i++){
		n.push_back(b[i].board);
	}
}

static void addToWriteData(std::vector<Board>& b, int spawn){
	std::lock_guard lock(writeData_mutex);
	for(int i = 0; i < b.size(); i++){
		if(spawn == 2)
			n2.push_back(b[i].board);
		if(spawn == 4)
			n4.push_back(b[i].board);
	}
}

static void addToWriteData(std::vector<boardSpawn>& b){
	std::lock_guard lock(writeData_mutex);
	for(int i = 0; i < b.size(); i++){
		if(b[i].spawn == 1){
			n2.push_back(b[i].b.board);
		}
		if(b[i].spawn == 2){
			n4.push_back(b[i].b.board);
		}
	}
}

static BS::multi_future<std::vector<Board>>* processSpawns(int spawn){
	const int size = n.size();
	std::unique_lock lk(pool_mutex);
	const int currentSum = globalsum;
	auto ret = new BS::multi_future<std::vector<Board>>
	(q.submit_blocks(0, size, [spawn](const int start, const int end){

		Logger logger;
		std::vector<Board> boards;
		std::vector<Board>* spawns;
		Board curr;
		for(auto i = start; i < end; i++){
			curr.board = n[i];
			if(satisfied(curr)){
				logger.log("Board is a winstate, skipping", Logger::INFO);
				continue;
			}
			spawns = genSpawns(curr, spawn);

			for(int j = 0; j < spawns->size(); j++){
				if(visited->visit((*spawns)[j].board))
					continue;
				boards.push_back((*spawns)[j]);
			}
			delete spawns;
		}
		return boards;
	}));
	return ret;
}

static BS::multi_future<std::vector<Board>>* processMoves(){
	const int size = n.size();
	std::unique_lock lk(pool_mutex);
	auto ret = new BS::multi_future<std::vector<Board>> (
	q.submit_blocks(0, size, [](const int start, const int end){
		Logger logger;
		std::vector<Board> boards;
		Board curr;
		std::vector<Board>* moves;
		for(auto i = start; i < end; i++){
			curr.board = n[i];
			if(satisfied(curr)){
				logger.log("Board is a winstate, skipping", Logger::INFO);
				continue;
			}
			moves = genMoves(curr);

			if(!(*moves).size())
				continue;

			for(int j = 0; j < (*moves).size(); j++){
				if(visited->visit((*moves)[j].board))
					continue;
				boards.push_back((*moves)[j]);
			}
			delete moves;
		}
		return boards;
	}));
	return ret;
}

void processBoard(){
	Logger logger;
	BS::multi_future<std::vector<Board>>* moves = processMoves();
	
	std::vector<Board> currBlock;
	int moveNum = 0;
	int spawnNum = 0;
	(*moves).wait();
	for(int i = 0; i < (*moves).size(); i++){
		currBlock = (*moves)[i].get();
		addToWriteData(currBlock);
		moveNum += currBlock.size();
	}
	delete moves;

	BS::multi_future<std::vector<Board>>* spawns2 = processSpawns(1);
	BS::multi_future<std::vector<Board>>* spawns4 = processSpawns(2);

	(*spawns2).wait();
	(*spawns4).wait();
	for(int i = 0; i < (*spawns2).size(); i++){
		currBlock = (*spawns2)[i].get();
		addToWriteData(currBlock, 2);
		spawnNum += currBlock.size();

		currBlock = (*spawns4)[i].get();
		addToWriteData(currBlock, 4);
		spawnNum += currBlock.size();
	}
	delete spawns2;
	delete spawns4;
}

static void writeBoards(int sum, std::vector<Board>& positions2){
	Logger logger;
	std::string fileName = "./" + config["positionsdir"] + "/" + std::to_string(sum + 2) + ".tables";
	std::ofstream file;
	file.open(fileName, std::ios::binary);
	if(!file.is_open()){
		logger.log("Could not open file for writing: " + fileName, Logger::FATAL);
		exit(0);
	}
	for(int i = 0; i < positions2.size(); i++)
		file.write(reinterpret_cast<char*>(&positions2[i].board), sizeof(uint64_t));
}

void gen_positions(Board& b, const int sum){
	static int s0 = std::stoi(config["s0"]);
	const auto start = std::chrono::system_clock::now();
	Logger logger;
	logger.log("Initializing safe map...", Logger::INFO);
	auto v = safe_map<uint64_t>(100000000, 5);
	logger.log("Done.", Logger::INFO);
	visited = &v;
	logger.log("Starting board:", Logger::INFO);
	outputBoard(b);
	writeB(
#ifdef COPY_BOARDS
			n, 
#endif
			true, sum);
	n.clear();
	n2.clear();
	n4.clear();
	n.push_back(b.board);

	while(n.size() && globalsum < s0){
		processBoard();
		q.wait();
#ifdef COPY_BOARDS
		auto nCpy = n;
		q.detach_task([nCpy]{
#endif
			writeB(
#ifdef COPY_BOARDS
					nCpy
#endif
					);
#ifdef COPY_BOARDS
		});
#endif
		n.clear();
		n = n2;
		n2 = n4;
		n4.clear();
		globalsum += 2;
	}
	writeB(
#ifdef COPY_BOARDS
			n
#endif
			);
	writeB(
#ifdef COPY_BOARDS
			n
#endif
			);
	q.wait();

	const auto end = std::chrono::system_clock::now();
	logger.log("Time: " + std::to_string((end - start) / 1s), Logger::INFO);
}
