#include "../include/maps.hpp"
#include "../include/error.hpp"
#include <cmath>
char MAP(int x){
	int log = std::log(x)/std::log(2);
	switch (log){
		case 0:
			return '0';
			break;
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
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			return 'a' + (log - 10);
	}
	return '0';
}
int LINEAR(int x){
	return x == 0 ? 0 : std::pow(2, x);
}
std::string COMMON(int x){
	int n = (x == 0 ? 0 : std::pow(2, x));
	switch(x){
		case 0:
			return "0";
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			return std::to_string(n);
		case 10:
			return "1k";
		case 11:
			return "2k";
		case 12:
			return "4k";
		case 13:
			return "8k";
		case 14:
			return "16k";
		case 15:
			return "32k";
	}
	return "0";
}
int SPAM(char x){
	int n = std::pow(2, (x >= 'A') ? (x >= 'a') ? (x - 'a' + 10) : (x - 'A' + 10) : (x - '0'));
	if (n > 1)
		return n;
	return 0;
}
Board strToBoard(std::string str){
	Logger logger;
	Board b;
	if(str.length() >= 16){
		logger.log("Board string longer than 16 characters!", Logger::NONFATAL);
	}
	for(size_t i = 0; i < str.length(); i++){
		b[i] = LINEAR(str[i]); // TODO: change to log
	}
	return b;
}
