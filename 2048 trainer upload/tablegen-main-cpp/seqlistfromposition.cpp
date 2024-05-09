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
static void writeB(std::shared_ptr<phmap::parallel_flat_hash_map<uint64_t, bool>> voisited = visited, bool setSum = false, int sumIn = 0){
	static long long unsigned boardNum;
	static int sum;
	static phmap::parallel_flat_hash_map<int, bool> sumWritten;
	static int lastLayerSize = 1;
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
	logger.log("Layer multiplier:  " + std::to_string(n.size() / lastLayerSize), Logger::INFO);
	lastLayerSize = n.size();
	logger.log("Layer size:  " + std::to_string(n.size()), Logger::INFO);
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
	logger.log("n[0]: " + std::to_string(getSum(n[0])), Logger::DEBUG);
	outputBoard(n[0]);
//	logger.log("n2[0]: " + std::to_string(getSum(n2[0])), Logger::DEBUG);
//	outputBoard(n2[0]);
//	logger.log("n4[0]: " + std::to_string(getSum(n4[0])), Logger::DEBUG);
//	outputBoard(n4[0]);
	n.clear();
	n = n2;
	n2 = n4;
	guard.unlock();
	sum += 2;
	globalsum = sum;
	lastWrite = std::chrono::system_clock::now();
}
static void checkForBadBoards(){
	Logger logger;
	for(int i = 0; i < n.size(); i++){
		if(getSum(n[i]) != globalsum){
			logger.log("Found bad root board:", Logger::FATAL);
			outputBoard(n[i]);
			exit(1);
		}
	}
}
static bool checkForBadBoard(Board b){
	Logger logger;
	if(getSum(b) != globalsum){
		logger.log("Found bad board:", Logger::FATAL);
		outputBoard(b);
		return true;
	}
	return false;
}
static void checkForBadBoards(std::vector<Board> b){
	Logger logger;
	for(int i = 0; i < n.size(); i++){
		if(getSum(b[i]) != globalsum){
			logger.log("Found bad root board:", Logger::FATAL);
			outputBoard(b[i]);
			exit(1);
		}
	}
}
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

std::shared_ptr<std::vector<Board>> genSpawns(Board b, int n){
	std::vector<Board> ret;
	Logger logger;
	Board boardCopy = b;
	for(int i = 0; i < std::stoi(config["spaces"]); i++){
		if(boardCopy[i] == 0){
			boardCopy[i] = n;
			ret.push_back(boardCopy);
			boardCopy = b;
		}
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

static bool visitBoard(Board b){
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
	for(int i = 0; i < b.size(); i++){
		if(!visitBoard(b[i]))
			continue;
		n.push_back(b[i]);
	}
	visited_mutex.unlock();
}

static void addToWriteData(std::vector<Board>& b, int spawn){
	std::lock_guard lock(writeData_mutex);
	for(int i = 0; i < b.size(); i++){
		if(!visitBoard(b[i]))
			continue;
		if(spawn == 2)
			n2.push_back(b[i]);
		if(spawn == 4)
			n4.push_back(b[i]);
	}
	visited_mutex.unlock();
}

static void addToWriteData(std::vector<boardSpawn>& b){
	std::lock_guard lock(writeData_mutex);
	for(int i = 0; i < b.size(); i++){
		if(!visitBoard(b[i].b))
			continue;
		visited_mutex.unlock();
		if(b[i].spawn == 1){
			n2.push_back(b[i].b);
		}
		if(b[i].spawn == 2){
			n4.push_back(b[i].b);
		}
	}
	visited_mutex.unlock();
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
		for(int i = start; i < end; i++){
			writeData_mutex.lock();
			auto spawns2 = genSpawns(n[i], 1);
			writeData_mutex.unlock();
			for(int j = 0; j < spawns2->size(); j++)
				boards.push_back({(*spawns2)[j], 1});
			writeData_mutex.lock();
			auto spawns4 = genSpawns(n[i], 2);
			writeData_mutex.unlock();
			for(int j = 0; j < spawns4->size(); j++){
					boards.push_back({(*spawns2)[j], 2});
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
			writeData_mutex.lock();
			auto moves = genMoves(n[i]);
			if(!(*moves).size())
				continue;
			for(int j = 0; j < (*moves).size(); j++){
				boards.push_back((*moves)[j]);
			}
			writeData_mutex.unlock();
		}
		logger.log("Checking output", Logger::DEBUG, 1);
		checkForBadBoards(boards);
		return boards;
	}));
}

void processBoard(BS::thread_pool& q){
	Logger logger;
	auto moves = processMoves(q);

	(*moves).wait();
	std::vector<boardSpawn> currSpawnBlock;
	std::vector<Board> currMoveBlock;
	int moveNum = 0;
	int spawnNum = 0;
	logger.log("Checking input boards", Logger::DEBUG);
	checkForBadBoards();
	for(int i = 0; i < (*moves).size(); i++){
		currMoveBlock = (*moves)[i].get();
		addToWriteData(currMoveBlock);
		moveNum += currMoveBlock.size();
	}
	logger.log("Checking global output boards", Logger::DEBUG);
	logger.log("Current sum:" + std::to_string(globalsum), Logger::DEBUG);
	checkForBadBoards();
	(*moves).clear();

	q.wait();
	auto spawns = processSpawns(q);

	(*spawns).wait();
	for(int i = 0; i < (*spawns).size(); i++){
		currSpawnBlock = (*spawns)[i].get();
		addToWriteData(currSpawnBlock);
		spawnNum += currSpawnBlock.size();
	}
	logger.log("Spawns: " + std::to_string(spawnNum), Logger::INFO);
	(*spawns).clear();
	logger.log("Root boards: " + std::to_string(n2.size()), Logger::INFO);
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
	const auto start = std::chrono::system_clock::now();
	BS::thread_pool q = BS::thread_pool(1);
	phmap::parallel_flat_hash_map<uint64_t, bool> voisited;
	visited = std::make_shared<phmap::parallel_flat_hash_map<uint64_t,bool>>(voisited);
	auto ptr = visited;
	Logger logger;

	logger.log("Starting board:", Logger::INFO);
	outputBoard(b);
	writeB(ptr, true, 518);
	n.clear();
	n2.clear();
	n4.clear();
	n.push_back(b);

	while(n.size()){
		processBoard(q);
		q.wait();
		writeB(ptr);
	}
	q.wait();

	const auto end = std::chrono::system_clock::now();
	logger.log("Time: " + std::to_string((end - start) / 1s), Logger::INFO);
}
