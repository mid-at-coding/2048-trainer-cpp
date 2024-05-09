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
static std::vector<Board> n;
static std::vector<Board> n2;
static std::vector<Board> n4;
static std::mutex writeData_mutex;
static safe_map visited;
static std::mutex pool_mutex;
static BS::thread_pool q = BS::thread_pool();
static void writeB(bool setSum = false, int sumIn = 0){
	static long long unsigned boardNum = 0;
	static long long unsigned layerNum = 0;
	static int sum;
	static phmap::parallel_flat_hash_map<int, bool> sumWritten;
	static int lastLayerSize = 1;
	static int s0 = std::stoi(config["s0"]);
	Logger logger;
	std::ofstream file;
	std::string fileName;
	static auto firstWrite = std::chrono::system_clock::now();
	static auto lastWrite = std::chrono::system_clock::now();
	auto now = std::chrono::system_clock::now();
	layerNum++;
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
	logger.log("Layer multiplier:  " + std::to_string((double)n.size() / lastLayerSize), Logger::INFO);
	lastLayerSize = n.size();
	logger.log("Layer size:  " + std::to_string(n.size()), Logger::INFO);
	if((now - lastWrite) / 1s != 0)
		logger.log("Layers per second: " + std::to_string(1.0f / ((now - lastWrite) / 1s)), Logger::INFO);
	if((now - lastWrite) / 1s != 0)
		logger.log("Boards per second: " + std::to_string(n.size() / ((now - lastWrite) / 1s)), Logger::INFO);
	if((now - firstWrite) / 1s != 0)
		logger.log("Average boards per second: " + std::to_string(boardNum / ((now - firstWrite) / 1s)), Logger::INFO);
	if((now - firstWrite) / 1s != 0){
		logger.log("Average layers per second: " + std::to_string(((double)layerNum) / ((now - firstWrite) / 1s)), Logger::INFO);
		logger.log("Estimated time remaining: " + std::to_string((int)((s0 - globalsum) / (((double)layerNum) / ((now - firstWrite) / 1s)))) + " seconds", Logger::INFO);
	}

	fileName = "./" + config["positionsdir"] + "/" + std::to_string(sum) + ".tables";
	file.open(fileName);
	if(!file.is_open()){
		logger.log("Could not open file: " + fileName, Logger::FATAL);
		exit(1);
	}
	std::unique_lock guard(writeData_mutex);
	for(int i = 0; i < n.size(); i++){
		file.write(reinterpret_cast<char*>(&n[i].board), sizeof(uint64_t));
		visited.clear(n[i].board);
	}
	n.clear();
	n = n2;
	n2 = n4;
	n4.clear();
	guard.unlock();
	sum += 2;
	globalsum = sum;
	lastWrite = std::chrono::system_clock::now();
}
static void checkForBadBoards(){
	Logger logger;
	for(int i = 0; i < n.size(); i++){
		if(getSum(n[i]) != globalsum){
			logger.log("Found bad root board:(" + std::to_string(getSum(n[i])) + ")", Logger::FATAL);
			outputBoard(n[i]);
			exit(1);
		}
	}
	for(int i = 0; i < n2.size(); i++){
		if(getSum(n2[i]) != globalsum + 2){
			logger.log("Found bad +2 board:(" + std::to_string(getSum(n2[i])) + ")", Logger::FATAL);
			outputBoard(n2[i]);
			exit(1);
		}
	}
	for(int i = 0; i < n4.size(); i++){
		if(getSum(n4[i]) != globalsum + 4){
			logger.log("Found bad +4 board:(" + std::to_string(getSum(n4[i])) + ")", Logger::FATAL);
			outputBoard(n4[i]);
			exit(1);
		}
	}
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
		if(GET_TILE(boardCopy.board, i) == 0){
			SET_TILE(boardCopy.board, i, n);
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
	return !visited[b.board];
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
		n.push_back(b[i]);
	}
}

static void addToWriteData(std::vector<Board>& b, int spawn){
	std::lock_guard lock(writeData_mutex);
	for(int i = 0; i < b.size(); i++){
		if(spawn == 2)
			n2.push_back(b[i]);
		if(spawn == 4)
			n4.push_back(b[i]);
	}
}

static void addToWriteData(std::vector<boardSpawn>& b){
	std::lock_guard lock(writeData_mutex);
	for(int i = 0; i < b.size(); i++){
		if(b[i].spawn == 1){
			n2.push_back(b[i].b);
		}
		if(b[i].spawn == 2){
			n4.push_back(b[i].b);
		}
	}
}

static std::shared_ptr<BS::multi_future<std::vector<Board>>> processSpawns(int spawn){
	const int size = n.size();
	std::unique_lock lk(pool_mutex);
	static const auto ptr = visited;
	const int currentSum = globalsum;
	return std::make_shared<BS::multi_future<std::vector<Board>>>
	(q.submit_blocks(0, size, [spawn](const int start, const int end){
		Logger logger;
		std::vector<Board> boards;
		for(int i = start; i < end; i++){
			if(satisfied(n[i]))
				continue;
			auto spawns = genSpawns(n[i], spawn);

			for(int j = 0; j < spawns->size(); j++){
				if(!visitBoard((*spawns)[j]))
					continue;
				boards.push_back((*spawns)[j]);
			}
		}
		return boards;
	}));
}

static std::shared_ptr<BS::multi_future<std::vector<Board>>> processMoves(){
	const int size = n.size();
	std::unique_lock lk(pool_mutex);
	return std::make_shared<BS::multi_future<std::vector<Board>>>(
	q.submit_blocks(0, size, [](const int start, const int end){
		Logger logger;
		std::vector<Board> boards;
		for(int i = start; i < end; i++){
			if(satisfied(n[i]))
				continue;
			auto moves = genMoves(n[i]);

			if(!(*moves).size())
				continue;

			for(int j = 0; j < (*moves).size(); j++){
				if(!visitBoard((*moves)[j]))
					continue;
				boards.push_back((*moves)[j]);
			}
		}
		return boards;
	}));
}

void processBoard(){
	Logger logger;
	std::shared_ptr<BS::multi_future<std::vector<Board>>> moves = processMoves();
	
	(*moves).wait();
	std::vector<Board> currBlock;
	int moveNum = 0;
	int spawnNum = 0;
	for(int i = 0; i < (*moves).size(); i++){
		currBlock = (*moves)[i].get();
		addToWriteData(currBlock);
		moveNum += currBlock.size();
	}
	(*moves).clear();

	std::shared_ptr<BS::multi_future<std::vector<Board>>> spawns2 = processSpawns(1);
	std::shared_ptr<BS::multi_future<std::vector<Board>>> spawns4 = processSpawns(2);

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
	(*spawns2).clear();
	(*spawns4).clear();
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
	Logger logger;

	logger.log("Starting board:", Logger::INFO);
	outputBoard(b);
	writeB(true, 518);
	n.clear();
	n2.clear();
	n4.clear();
	n.push_back(b);

	while(n.size() && globalsum < std::stoi(config["s0"])){
		processBoard();
		q.wait();
		writeB();
	}
	writeB();
	writeB();
	q.wait();

	const auto end = std::chrono::system_clock::now();
	logger.log("Time: " + std::to_string((end - start) / 1s), Logger::INFO);
}
