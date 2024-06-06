#include <string>
#include <optional>
#include "../include/error.hpp"
#include "../include/ui.hpp"

std::optional<int> getInt(){
	Logger logger;
	std::string inp,raw;
	std::optional<int> ret;
	std::getline(std::cin, raw);
	for(int i = 0; i < raw.size(); i++){
		if(raw[i] == ' ')
			continue;
		inp += std::toupper(raw[i]);
	}
	try{
		ret = stoi(inp);
	}
	catch(std::invalid_argument){
		return ret;
	}
	return ret;
}

std::optional<bool> getBool(){
	Logger logger;
	std::string inp,raw;
	std::optional<bool> ret;
	std::getline(std::cin, raw);
	for(int i = 0; i < raw.size(); i++){
		if(raw[i] == ' ')
			continue;
		inp += std::toupper(raw[i]);
	}
	if(inp == "Y")
		ret = true;
	if(inp == "YES")
		ret = true;
	if(inp == "TRUE")
		ret = true;
	if(inp == "T")
		ret = true;
	if(inp == "N")
		ret = false;
	if(inp == "NO")
		ret = false;
	if(inp == "FALSE")
		ret = false;
	if(inp == "F")
		ret = false;
	return ret;
}
void setBool(bool *ret){
	Logger logger;
	auto input = getBool();
	if(input.has_value()){
		logger.log("changed to " + std::to_string(input.value()), Logger::DEBUG);
		(*ret) = input.value();
	}
}
void setInt(int *ret){
	Logger logger;
	auto input = getInt();
	if(input.has_value()){
		logger.log("changed to " + std::to_string(input.value()), Logger::DEBUG);
		(*ret) = input.value();
	}
}

int getNumberInRange(unsigned floor, unsigned ceil, std::string valueName){
	Logger logger;
	int cores = -1;
	logger.log("Input " + valueName, Logger::INFO);
	setInt(&cores);
	while(cores == -1 || cores > ceil || cores < floor){
		logger.log("Invalid " + valueName, Logger::INFO);
		logger.log("Input " + valueName, Logger::INFO);
		setInt(&cores);
	}
	return cores;
}

