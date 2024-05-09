#ifndef VISITED_H
#define VISITED_H
#include <stdint.h>
#define SAFE_MAP_SIZE 5000000029
class safe_map{
	bool* map;
public:	
	safe_map();
	bool operator[](uint64_t b);
	void clear();
	void clear(uint64_t b);
};

#endif
