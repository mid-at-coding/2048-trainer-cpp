#include "board.hpp"
#include "error.hpp"
#include "config.hpp"
#include "BS_thread_pool.hpp" // gh.com/bshoshany/thread_pool
#include <cstdint>
#include "movedir.hpp"
#include <fstream>
#include "unordered_dense/include/ankerl/unordered_dense.h"
#include "parallel-hashmap/parallel_hashmap/phmap.h"
#include "seqlistfromposition.hpp"
#include "maps.hpp"
#include <iomanip>
#include <boost/sort/sort.hpp>
#include <mutex>
#include "satisfied.hpp"
#include <tbb/tbb.h>
#define DPDF
#define COPY_BOARDS
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
static std::mutex pool_mutex;
static BS::thread_pool q;
static int comp(const void* a, const void* b){
	if((*(uint64_t*)a) == (*(uint64_t*)b))
		return 0;
	if((*(uint64_t*)a) > (*(uint64_t*)b))
		return 1;
	return -1;
}

static void writeB(std::vector<uint64_t> boards, int sum){
	std::string fileName;
	static long long unsigned boardNum;
	static long long unsigned lastLayerSize;
	static auto firstWrite = std::chrono::system_clock::now();
	auto now = std::chrono::system_clock::now();
	std::ofstream file;
	Logger logger;
	boardNum += boards.size();
	logger.log("Sum: " + std::to_string(sum), Logger::INFO);
	if((now - firstWrite) / 1s != 0)
		logger.log("Average boards per second: " + std::to_string(boardNum / ((now - firstWrite) / 1s)), Logger::INFO);
	if(n.size() - lastLayerSize)
		logger.log("Layer multiplier:  " + std::to_string((double)boards.size() / lastLayerSize), Logger::INFO);
	lastLayerSize = boards.size();
	logger.log("Layer size:  " + std::to_string(boards.size()), Logger::INFO);
	fileName = "./" + config["positionsdir"] + "/" + std::to_string(sum) + ".tables";
	file.open(fileName);
	if(!file.is_open()){
		logger.log("Could not open file: " + fileName, Logger::FATAL);
		exit(1);
	}
	for(int i = 0; i < boards.size(); i++){
#ifndef DRY_RUN
		file.write(reinterpret_cast<char*>(&(boards[i])), sizeof(uint64_t));
#endif
	}
}

static void writeB( bool setSum = false, int sumIn = 0){
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
	Logger logger;
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
		if(spawn == 1)
			n2.push_back(b[i].board);
		if(spawn == 2)
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

static std::vector<uint64_t>* processSpawns(int spawn){
	Logger logger;
	const int size = n.size();
	const int currentSum = globalsum;
//	pool_mutex.lock();
#ifdef COPY_BOARDS
	auto nCpy = new std::vector<uint64_t>();
	(*nCpy) = n;
#endif
	q.detach_blocks(0, size, [spawn, nCpy](const int start, const int end){

		Logger logger;
		std::vector<Board> boards;
		std::vector<Board>* spawns;
		Board curr;
		for(auto i = start; i < end; i++){
			curr.board = (*nCpy)[i]; // segfault
			if(satisfied(curr)){
				logger.log("Board is a winstate, skipping", Logger::INFO);
				continue;
			}
			spawns = genSpawns(curr, spawn);

			for(int j = 0; j < spawns->size(); j++){
//				if((*visited)[(*spawns)[j].board] == true)
//					continue;
//				(*visited)[(*spawns)[j].board] = true;
				boards.push_back((*spawns)[j]);
			}
			delete spawns;
		}
		addToWriteData(boards, spawn);
	});
//	pool_mutex.unlock();
	return nCpy;
}

static std::vector<uint64_t>* processMoves(){
	Logger logger;
//	pool_mutex.lock();
	const int size = n.size();
#ifdef COPY_BOARDS
	auto nCpy = new std::vector<uint64_t>();
	(*nCpy) = n;
#endif
	q.detach_blocks(0, size, [nCpy](const int start, const int end){
		Logger logger;
		std::vector<Board> boards;
		Board curr;
		std::vector<Board>* moves;
		for(auto i = start; i < end; i++){
			curr.board = (*nCpy)[i];
			if(satisfied(curr)){
				logger.log("Board is a winstate, skipping", Logger::INFO);
				continue;
			}
			moves = genMoves(curr);

			if(!(*moves).size())
				continue;

			for(int j = 0; j < (*moves).size(); j++){
//				if((*visited)[(*moves)[j].board] == true)
//					continue;
//				(*visited)[(*moves)[j].board] = true;
				boards.push_back((*moves)[j]);
			}
			delete moves;
		}
		addToWriteData(boards);
	});
//	pool_mutex.unlock();
	return nCpy;
}

void processBoard(){
	Logger logger;

	deleteDupes(&n);
	auto nCpy = processMoves();
	q.wait();
	delete nCpy;
	deleteDupes(&n);

	nCpy = processSpawns(1);
	auto nCpy2 = processSpawns(2);
	q.wait();
	deleteDupes(&n4);
	deleteDupes(&n2);
	delete nCpy;
	delete nCpy2;
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

void gen_positions(Board& b, const int sum, const int cores){
	static int s0 = std::stoi(config["s0"]);
	const auto start = std::chrono::system_clock::now();
	Logger logger;
	q.reset(cores);
	logger.log("Starting board:", Logger::INFO);
	outputBoard(b);
#ifndef COPY_BOARDS
	writeB( true, sum );
#endif
	n.clear();
	n2.clear();
	n4.clear();
	n.push_back(b.board);
	auto nCpy = n;
	globalsum = sum;
	int currentSum = globalsum;

	while(n.size() && globalsum < s0){
		processBoard();
#ifdef COPY_BOARDS
		nCpy = n;
		currentSum = globalsum;
//		pool_mutex.lock();
		q.detach_task([&nCpy, currentSum]{
#endif
#ifndef DRY_RUN
			writeB(
#ifdef COPY_BOARDS
					nCpy, currentSum
#endif
					);
#endif
#ifdef COPY_BOARDS
		});
#endif
		n.clear();
		n = n2;
		n2 = n4;
		n4.clear();
		globalsum += 2;
#ifdef COPY_BOARDS
//		pool_mutex.unlock();
#endif
	}
	writeB(
#ifdef COPY_BOARDS
			n2, globalsum
#endif
			);
	writeB(
#ifdef COPY_BOARDS
			n4, globalsum + 2
#endif
			);
	q.wait();

	const auto end = std::chrono::system_clock::now();
	logger.log("Time: " + std::to_string((end - start) / 1s), Logger::INFO);
}

