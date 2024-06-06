#include "../include/board.hpp"
#include "../include/error.hpp"
#include "../include/config.hpp"
#include "../include/BS_thread_pool.hpp" // gh.com/bshoshany/thread_pool
#include <cstdint>
#include "../include/movedir.hpp"
#include <fstream>
#include "../include/unordered_dense/include/ankerl/unordered_dense.h"
#include "../include/parallel-hashmap/parallel_hashmap/phmap.h"
#include "../include/seqlistfromposition.hpp"
#include "../include/maps.hpp"
#include <iomanip>
#include <boost/sort/sort.hpp>
#include <mutex>
#include "../include/satisfied.hpp"
#include <tbb/tbb.h>
#define DPDF
using namespace std::chrono_literals;
struct boardSpawn{
	Board b;
	int spawn;
};
static int globalsum;
static std::vector<uint64_t>* n;
static std::vector<uint64_t>* n2;
static std::vector<uint64_t>* n4;
static std::mutex writeData_mutex;
static std::mutex pool_mutex;
static BS::thread_pool q;

static void writeB(std::vector<uint64_t>* boards, int sum){
	std::string fileName;
	static long long unsigned boardNum;
	static long long unsigned lastLayerSize;
	static const auto firstWrite = std::chrono::system_clock::now();
	static auto lastWrite = std::chrono::system_clock::now();
	static const int s0 = std::stoi(config["s0"]);
	static int layerNum = 0;
	auto now = std::chrono::system_clock::now();
	std::ofstream file;
	Logger logger;
	layerNum++;

	boardNum += boards->size();
	logger.log("Sum: " + std::to_string(sum), Logger::INFO);
	if((now - firstWrite) / 1s != 0){
		logger.log("Average boards per second(mBps): " + std::to_string((float)((float)boardNum / ((now - firstWrite) / 1s)) / 1000000.0f), Logger::INFO);
		logger.log("Average layers per second: " + std::to_string(((double)layerNum) / ((now - firstWrite) / 1s)), Logger::INFO);
		logger.log("Estimated time remaining: " + std::to_string((int)((s0 - globalsum) / (((double)layerNum) / ((1/60.0f) * ((now - firstWrite) / 1s))))) + " minutes", Logger::INFO);
	}
	if(n->size() - lastLayerSize)
		logger.log("Layer multiplier:  " + std::to_string((double)boards->size() / lastLayerSize), Logger::INFO);
	if((now - lastWrite) / 1s != 0)
		logger.log("Layers per second: " + std::to_string((double)1 / (double)((now - lastWrite) / 1000ms)), Logger::INFO);

	lastLayerSize = boards->size();
	logger.log("Layer size:  " + std::to_string(boards->size()), Logger::INFO);
	fileName = "./" + config["positionsdir"] + "/" + std::to_string(sum) + ".tables";

	file.open(fileName);
	if(!file.is_open()){
		logger.log("Could not open file: " + fileName, Logger::FATAL);
		exit(1);
	}
	for(size_t i = 0; i < boards->size(); i++){
#ifndef DRY_RUN
		file.write(reinterpret_cast<char*>(&((*boards)[i])), sizeof(uint64_t));
#endif
	}
	lastWrite = std::chrono::system_clock::now();
}

void outputBoard(Board& b){
	int pos = 0;
	for(int i = 0; i < 16; i++){
		if(
#ifdef TWELVE_SPACE
				i == 10 || i == 11 || i == 14 || i == 15
#endif
#ifdef DPDF
				i == 10 || i == 11 || i == 13 || i == 14 || i == 15
#endif
				)
			std::cout << std::setw(3) << ("x");
		else
			std::cout << std::setw(3) << (COMMON(b[pos++]));
		std::cout << " " ;
		if(!((i + 1) % 4))
			std::cout << std::endl;
	}
	std::cout << std::endl;
}
void outputBoard(uint64_t& b){
	int pos = 0;
	for(int i = 0; i < 16; i++){
		if(i == 10 || i == 11 || i == 14 || i == 15)
			std::cout << std::setw(3) << ("x");
		else{
			std::cout << std::setw(3) << (COMMON(GET_TILE(b, pos)));
			pos++;
		}
		std::cout << " " ;
		if(!((i + 1) % 4))
			std::cout << std::endl;
	}
	std::cout << std::endl;
}
static void deleteDupes(std::vector<uint64_t>* boards){
// 	SET
/*	phmap::parallel_flat_hash_set<uint64_t, phmap::priv::hash_default_hash<uint64_t>, phmap::priv::hash_default_eq<uint64_t>, phmap::priv::Allocator<uint64_t>, 4, std::mutex> s;
	const auto size = boards->size();
	q.submit_blocks(0lu, size, [boards, &s](const int start, const int end){
		for(int i = start; i < end; i++)
			s.insert((*boards)[i]);
	}).wait();
	boards->assign(s.begin(), s.end()); */
//	SORT
	boost::sort::block_indirect_sort(boards->begin(), boards->end());
	boards->erase(std::unique(boards->begin(), boards->end()), boards->end());
//	CONVOLUTION
/*	const auto size = boards->size();
	std::vector<int> curr;
	int last;
	BS::multi_future<std::vector<int>> res = q.submit_blocks(1lu, size, [boards](const int start, const int end){
		std::vector<int> ret;
		for(int i = start; i < end; i++)
			ret.push_back((*boards)[i] - (*boards)[i - 1]);
		return ret;
	});
	for(int i = 0; i < boards->size(); i++){
		if(curr.size() < i - last)
			curr = res[i].get();
		if(curr[last - i] == 0) // this board is equal to the last one
			boards->erase(boards->begin() + i);
	} */
}
int getSum(Board& b){
	int s = 0;
	for(int i = 0; i < 16; i++){
		s += LINEAR(b[i]);
	}
	return s;
}

void genSpawns(const Board& b, int n, std::vector<Board>* ret){
	static int spaces = std::stoi(config["spaces"]);
	Board boardCopy = b;
	for(int i = 0; i < spaces; i++){
		if(GET_TILE(boardCopy.board, i) == 0){
			SET_TILE(boardCopy.board, i, n);
			ret->push_back(boardCopy);
			boardCopy = b;
		}
	}
}

static void genMoves(const Board& b, std::vector<Board>* ret){
	Board boardCopy = b;
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
}

static void addToWriteData(std::vector<Board>& b){
	std::lock_guard lock(writeData_mutex);
	for(size_t i = 0; i < b.size(); i++){
		n->push_back(b[i].board);
	}
}

static void addToWriteData(std::vector<Board>& b, int spawn){
	std::lock_guard lock(writeData_mutex);
	for(size_t i = 0; i < b.size(); i++){
		if(spawn == 1)
			n2->push_back(b[i].board);
		if(spawn == 2)
			n4->push_back(b[i].board);
	}
}

static BS::multi_future<std::vector<Board>> processSpawns(const int& spawn){
	const size_t size = n->size();
	return q.submit_blocks((size_t)0, size, [spawn](const int start, const int end){
		Logger logger;
		std::vector<Board> boards;
		std::vector<Board>* spawns = new std::vector<Board>;
		Board curr;
		for(auto i = start; i < end; i++){
			curr.board = n->at(i); 
			if(satisfied(curr)){
				continue;
			}
			spawns->clear();
			genSpawns(curr, spawn, spawns);

			for(size_t j = 0; j < spawns->size(); j++){
				boards.push_back((*spawns)[j]);
			}
		}
		return boards;
	});
}

static BS::multi_future<std::vector<Board>> processMoves(){
	const int size = n->size();
	return q.submit_blocks(0, size, [](const int start, const int end){
		Logger logger;
		std::vector<Board> boards;
		Board curr;
		std::vector<Board>* moves = new std::vector<Board>;
		for(auto i = start; i < end; i++){
			curr.board = n->at(i);
			if(satisfied(curr)){
				logger.log("Board is a winstate, skipping", Logger::INFO);
				continue;
			}
			moves->clear();
			genMoves(curr, moves);

			if(!(*moves).size())
				continue;

			for(size_t j = 0; j < (*moves).size(); j++){
				boards.push_back((*moves)[j]);
			}
		}
		return boards;
	});
}

void processBoard(){
	static const int two = 1;
	static const int four = 2;
	deleteDupes(n);
	auto moves = processMoves();
	q.wait(); // wait for everything before adding results to n to avoid race cond
	for(std::future<std::vector<Board>>& b : moves){ // put moves into n
		std::vector<Board> curr = b.get();
		for(int i = 0; i < curr.size(); i++)
			n->push_back(curr[i].board);
	}
	deleteDupes(n);

	auto twoSpawns  =  processSpawns(two); // we can start spawning while we're writing
	auto fourSpawns = processSpawns(four);

	writeB(n, globalsum);

	q.wait();
	for(std::future<std::vector<Board>>& b : twoSpawns){ // put two spawns into n2
		std::vector<Board> curr = b.get();
		for(int i = 0; i < curr.size(); i++)
			n2->push_back(curr[i].board);
	}
	for(std::future<std::vector<Board>>& b : fourSpawns){ // put four spawns into n4
		std::vector<Board> curr = b.get();
		for(int i = 0; i < curr.size(); i++)
			n4->push_back(curr[i].board);
	}
	deleteDupes(n4);
	deleteDupes(n2);

	auto temp = n;
	n = n2;
	n2 = n4;
	n4 = temp;
	n4->clear();
	globalsum += 2;
}

void gen_positions(Board& b, const int sum, const int cores){
	static int s0 = std::stoi(config["s0"]);
	const auto start = std::chrono::system_clock::now(); // save the starting time
	Logger logger;
	q.reset(cores);

	logger.log("Starting board:", Logger::INFO);
	outputBoard(b);

	n = new std::vector<uint64_t>;
	n2 = new std::vector<uint64_t>;
	n4 = new std::vector<uint64_t>;
	n->push_back(b.board); // initialize vectors

	globalsum = sum;

	while(n->size() && globalsum < s0){
		processBoard();
	}
	writeB(n2, globalsum); // write the boards past the end that we have in memory
	writeB(n4, globalsum + 2);

	const auto end = std::chrono::system_clock::now();
	logger.log("Time: " + std::to_string((end - start) / 1s), Logger::INFO);
}

