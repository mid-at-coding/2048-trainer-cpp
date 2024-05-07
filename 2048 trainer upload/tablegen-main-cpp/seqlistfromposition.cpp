#include "board.hpp"
#include "error.hpp"
#include "config.hpp"
#define BS_THREAD_POOL_ENABLE_PAUSE
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
using namespace std::chrono_literals;
struct boardSpawn{
	Board b;
	int spawn;
};
static int globalsum;
static std::vector<Board> n;
static std::vector<Board> n2;
static std::vector<Board> n4;
static std::mutex writeData_mutex;
std::shared_ptr<phmap::parallel_flat_hash_map<uint64_t, bool>> visited;
static std::mutex visited_mutex;
static std::mutex pool_mutex;
void outputBoard(Board& b){
	for(int i = 0; i < 16; i++){
		std::cout << std::setw(3) << (COMMON(b[i]));
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
static bool genSpawn(Board& b, int n, int spawn){
	char currentSpace = 0;
	static int spaces = std::stoi(config["spaces"]);
	for(int i = 0; i < spaces; i++){  	// loop through tiles
		if (!b[i]){						// if current tile is empty
			if(n == currentSpace++){  	// and also if this is the tile we want to fill
				b[i] = spawn;			// fill it
				return true; 			// and return true
			}
		}
	}
	return false; // if we couldn't find the nth space, return false
}
std::shared_ptr<std::vector<Board>> genSpawns(Board b, int n){
	std::vector<Board> ret;
	Logger logger;
	Board boardCopy = b;
	int space = 0;
	while(genSpawn(boardCopy, space++, n)){
		ret.push_back(boardCopy);
		boardCopy = b;
	}
	return std::make_unique<std::vector<Board>>(ret);
}

static std::shared_ptr<std::vector<Board>> genMoves(Board& b){
	Logger logger;
	Board boardCopy = b;
	std::vector<Board> ret;
	if(moveleft(boardCopy)){
		ret.push_back(boardCopy);
		boardCopy = b;
	}
	if(moveright(boardCopy)){
		ret.push_back(boardCopy);
		boardCopy = b;
	}
	if(moveup(boardCopy)){
		ret.push_back(boardCopy);
		boardCopy = b;
	}
	if(movedown(boardCopy)){
		ret.push_back(boardCopy);
		boardCopy = b;
	}
	return std::make_unique<std::vector<Board>>(ret);
}

static bool visitBoard(Board b,  int sum = globalsum){
	Logger logger;
	if((*visited).contains(b.board))
		return false;
	visited_mutex.lock();
	(*visited)[b.board] = true;
	visited_mutex.unlock();
	return true;
}

static bool addToWriteData(Board b){
	if(!writeData_mutex.try_lock())
		return false;
	n.push_back(b);
	return true;
}

static void addToWriteData(std::vector<Board>& b){
	std::lock_guard lock(writeData_mutex);
	for(int i = 0; i < b.size(); i++)
		n.push_back(b[i]);
}
static void addToWriteData(std::vector<boardSpawn>& b){
	std::lock_guard lock(writeData_mutex);
	for(int i = 0; i < b.size(); i++){
		if(b[i].spawn == 1)
			n2.push_back(b[i].b);
		if(b[i].spawn == 2)
			n4.push_back(b[i].b);
	}
}

static std::shared_ptr<BS::multi_future<std::vector<boardSpawn>>> processSpawns(BS::thread_pool& q){
	const int size = n.size();
	std::unique_lock lk(pool_mutex);
	static const auto ptr = visited;
	const int currentSum = globalsum;
	return std::make_shared<BS::multi_future<std::vector<boardSpawn>>>
	(q.submit_blocks(0, size, [currentSum](const int start, const int end){
		Logger logger;
		std::vector<boardSpawn> boards;
		if(currentSum != globalsum) // ??
			logger.log("Saved sums are different, errors may occur", Logger::WARN);
		for(int i = start; i < end; i++){
			logger.log("Generating spawns from board:", Logger::DEBUG);
			outputBoard(n[i]);
			logger.log("sum(given) : " + std::to_string(currentSum), Logger::DEBUG);
			logger.log("sum(actual): " + std::to_string(getSum(n[i])), Logger::DEBUG);
			writeData_mutex.lock();
			if(getSum(n[i]) != currentSum){ // ??
				logger.log("Root sums are different, errors will occur(" + std::to_string(getSum(n[i])) + "," + std::to_string(currentSum) + ")", Logger::FATAL);
				outputBoard(n[i]);
				exit(1);
			}
			auto spawns2 = genSpawns(n[i], 1);
			auto spawns4 = genSpawns(n[i], 2);
			writeData_mutex.unlock();
			for(int j = 0; j < (*spawns2).size(); j++){
				if(visitBoard((*spawns2)[j], currentSum + 2))
					boards.push_back({(*spawns2)[j], 1});
				if(getSum((*spawns2)[j]) != currentSum + 2) // ??
					logger.log("Sums are different, errors may occur(" + std::to_string(getSum((*spawns2)[j])) + "," + std::to_string(currentSum + 2) + ")", Logger::WARN);
			}
			for(int j = 0; j < (*spawns4).size(); j++){
				if(visitBoard((*spawns4)[j], currentSum + 4))
					boards.push_back({(*spawns2)[j], 2});
				if(getSum((*spawns4)[j]) != currentSum + 4) // ??
					logger.log("Sums are different, errors may occur(" + std::to_string(getSum((*spawns4)[j])) + "," + std::to_string(currentSum + 4) + ")", Logger::WARN);
			}
		}
		return boards;
	}));
}

static std::shared_ptr<BS::multi_future<std::vector<Board>>> processMoves(BS::thread_pool& q){
	const int size = n.size();
	std::unique_lock lk(pool_mutex);
	return std::make_shared<BS::multi_future<std::vector<Board>>>(
	q.submit_blocks(0, size, [](const int start, const int end){
		Logger logger;
		std::vector<Board> boards;
		for(int i = start; i < end; i++){
			auto moves = genMoves(n[i]);
			if(!(*moves).size())
				continue;
			for(int j = 0; j < (*moves).size(); j++){
				if(visitBoard((*moves)[j]))
					boards.push_back((*moves)[j]);
			}
		}
		return boards;
	}));
}

static void write(std::shared_ptr<phmap::parallel_flat_hash_map<uint64_t, bool>> voisited, bool setSum = false, int sumIn = 0){
	static long long unsigned boardNum;
	static int sum;
	static phmap::parallel_flat_hash_map<int, bool> sumWritten;
	Logger logger;
	std::ofstream file;
	std::string fileName;
	static auto lastWrite = std::chrono::system_clock::now();
	auto now = std::chrono::system_clock::now();
	if(setSum){
		sum = sumIn;
		globalsum = sum;
		return;
	}
	if(sumWritten[sum])
		return;
	sumWritten[sum] = true;

	logger.log("Sum: " + std::to_string(sum), Logger::INFO);
	boardNum += n.size();
	logger.log("Boards processed:  " + std::to_string(boardNum), Logger::INFO);
	if((now - lastWrite) / 1s != 0)
		logger.log("Boards per second: " + std::to_string(n.size() / ((now - lastWrite) / 1s)), Logger::INFO);
	std::lock_guard vislk(visited_mutex);

	fileName = "./" + config["positionsdir"] + "/" + std::to_string(sum) + ".tables";
	file.open(fileName);
	if(!file.is_open()){
		logger.log("Could not open file: " + fileName, Logger::FATAL);
		exit(1);
	}
	std::unique_lock guard(writeData_mutex);
	for(int i = 0; i < n.size(); i++){
		file.write(reinterpret_cast<char*>(&n[i].board), sizeof(uint64_t));
		visited->erase(n[i].board);
	}
	n.clear();
	n = n2;
	n2 = n4;
	guard.unlock();
	sum += 2;
	globalsum = sum;
	lastWrite = std::chrono::system_clock::now();
}
void processBoard(BS::thread_pool& q){
	Logger logger;
	auto moves = processMoves(q);
	std::vector<Board> currBlock;
	(*moves).wait();
	for(int i = 0; i < (*moves).size(); i++)
		addToWriteData(currBlock = (*moves)[i].get());
	(*moves).clear();
	auto spawns = processSpawns(q);
	std::vector<boardSpawn> currSpawnBlock;
	(*spawns).wait();
	for(int i = 0; i < (*spawns).size(); i++)
		addToWriteData(currSpawnBlock = (*spawns)[i].get());
	q.wait();
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

void gen_positions(Board& b, int sum){
	Logger logger;
	logger.log("Starting board:", Logger::INFO);
	const auto start = std::chrono::system_clock::now();
	outputBoard(b);
	n.push_back(b);
	BS::thread_pool q = BS::thread_pool(1);
	phmap::parallel_flat_hash_map<uint64_t, bool> voisited;
	visited = std::make_shared<phmap::parallel_flat_hash_map<uint64_t,bool>>(voisited);
	auto ptr = visited;
	write(ptr, true, 518);
	while(n.size()){
		processBoard(q);
		q.wait();
		write(ptr);
	}
	q.wait();
	const auto end = std::chrono::system_clock::now();
	logger.log("Time: " + std::to_string((end - start) / 1s), Logger::INFO);
}
