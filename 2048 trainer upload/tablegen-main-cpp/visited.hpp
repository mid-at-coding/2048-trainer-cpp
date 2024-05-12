#ifndef VISITED_H
#define VISITED_H
#include <stdint.h>
#include <vector>
#include <cstdlib>
#include "komihash/komihash.h"
#include "error.hpp"
#include <mutex>

#define DEFAULT_RESIZE 5
#define MAX_RESIZE_LF 0.5

template<typename T>
class safe_map{
	std::vector<T>* map;
	std::mutex resizingMutex;
	unsigned long long buckets = 0;
	unsigned long long bucketSize = 0;
	int values = 0;
	void resize(int change = DEFAULT_RESIZE);
	bool findOrAdd(const T& b);
	bool findOrAdd(const unsigned long long& index);
	void resetBucket(const unsigned long long& index);
public:	
	/* This map is for implementing a fast, thread safe, hash map
	 * except for when the array is being resized, reading and writing is always thread safe
	 * it's hopefully not too slow, but that remains to be seen */
	safe_map();
	safe_map(const uint64_t& sizeIn, const uint64_t& bucketsIn);
	bool operator[](const T& b);
	bool visit(const T& b);
	void clear();
	void clear(const T& b);
};

template<typename T>
bool safe_map<T>::findOrAdd(const T& b){
	resizingMutex.lock();
	resizingMutex.unlock();
	T curr;
	if((double)values / (double)buckets > MAX_RESIZE_LF)
		resize();
	uint64_t index = komihash(&b, sizeof(T), 0); 
	index *= buckets;
	index = index % map->size();
	for(int i = index; i < index + bucketSize; i++){
		if(!resizingMutex.try_lock()){ // recalculate index
			uint64_t index = komihash(&b, sizeof(T), 0); 
			index *= buckets;
			index = index % map->size(); 
		}
		curr = (*map)[i];
		resizingMutex.unlock();
		if(curr == b){
			return true;
		}
		if(curr == T()){ // bucket has space
			(*map)[i] = b;
			values++;
			return false;
		}
	}
	resize(); // we didn't just find a board, so we must have to resize 
	resizingMutex.lock(); // wait to not be resizing
	resizingMutex.unlock();
	findOrAdd(b);
	return false;
}
template<typename T>
void safe_map<T>::resetBucket(const unsigned long long& index){
	for(int i = 0; i < bucketSize; i++){
		(*map)[i] = T();
	}
}
template<typename T>
void safe_map<T>::clear(){
	values = 0;
	for(uint64_t i = 0; i < map->size(); i++){
		(*map)[i] = T();
	}
}
template<typename T>
void safe_map<T>::clear(const T& b){
	uint64_t index = komihash(&b, 8, 0); 
	index *= buckets; 
	index = index % map->size(); 
	for(int i = index; i < index + bucketSize; i++){
		if((*map)[i] == b){
			(*map)[i] = T();
			values--;
			return;
		}
	}
}
template<typename T>
safe_map<T>::safe_map(){
	Logger logger;
	try{
		map = new std::vector<T>(1);
	}
	catch(std::bad_alloc){
		logger.log("Could not allocate safe map", Logger::FATAL);
		exit(1);
	}
	map->push_back(T());
	buckets = 1;
	bucketSize = 1;
	clear();
}
template<typename T>
safe_map<T>::safe_map(const uint64_t& sizeIn, const uint64_t& bucketSizeIn){
	buckets = sizeIn / bucketSizeIn;
	bucketSize = bucketSizeIn;
	Logger logger;
	logger.log("Buckets: " + std::to_string(buckets), Logger::DEBUG);
	logger.log("Bucket size: " + std::to_string(bucketSize), Logger::DEBUG);
	logger.log("Allocating " + std::to_string(sizeIn * sizeof(T)) + " bytes...", Logger::DEBUG);
	try{
		map = new std::vector<T>(sizeIn);
	}
	catch(std::bad_alloc){
		logger.log("Could not allocate safe map", Logger::FATAL);
		exit(1);
	}
	logger.log("Allocated.", Logger::DEBUG);
	for(int i = 0; i < map->size(); i++){
		(*map)[i] = T();
	}
	clear();
}
template<typename T>
bool safe_map<T>::operator[](const T& b){
	return findOrAdd(b);
}
template<typename T>
bool safe_map<T>::visit(const T& b){
	return findOrAdd(b);
}
template<typename T>
void safe_map<T>::resize(int change){
	Logger logger;
	if (!resizingMutex.try_lock()){
		logger.log("Can't resize", Logger::DEBUG);
		return; // don't resize if we're currently resizing
	}
	auto newMap = new std::vector<T>;
	std::vector<T> oldBucket;
	for(int i = 0; i < buckets; i++){
		for(int index = i * bucketSize; index < (i + 1) * bucketSize; index++)
			oldBucket.push_back((*map)[index]);
		for(int j = 0; j < oldBucket.size() && j < oldBucket.size() + change; j++)
			newMap->push_back(oldBucket[j]);
		for(int j = 0; j < change; j++)
			newMap->push_back(T());
		oldBucket.clear();
	}
	delete map;
	bucketSize += change;
	map = newMap;
	resizingMutex.unlock();
}

#endif
