#include "maps.hpp"
#include <cmath>
#include "error.hpp"

constexpr int SPAM(char x){
	int n = std::pow(2, (x >= 'A') ? (x >= 'a') ? (x - 'a' + 10) : (x - 'A' + 10) : (x - '0'));
	if (n > 1)
		return n;
	return 0;
}

constexpr int LINEAR(char x){
	return (x >= 'A') ? (x >= 'a') ? (x - 'a' + 10) : (x - 'A' + 10) : (x - '0');
}
constexpr char MAP(int x){
	int log = std::log(x)/std::log(2);
	switch (log){
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			return std::to_string(log)[0];
			break;
		default:
			return 'a' + (log - 10);
	}
	return 0;
}

Board strToBoard(std::string str){
	Logger logger;
	Board b;
	if(str.length() >= 16){
		logger.log("Board string longer than 16 characters!", Logger::NONFATAL);
	}
	for(int i = 0; i < str.length(); i++){
		b[i] = LINEAR(str[i]); // TODO: change to log
	}
	return b;
}
