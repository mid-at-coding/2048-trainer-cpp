#ifndef VISITED_H
#define VISITED_H
#include <stdint.h>
#define SAFE_MAP_SIZE 2147483647
class safe_map{
public:	
	int collisions;
	uint32_t* map;
	safe_map();
	bool operator[](const uint64_t& b);
	bool visit(const uint64_t& b);
	void clear();
	void clear(const uint64_t& b);
};

#endif
