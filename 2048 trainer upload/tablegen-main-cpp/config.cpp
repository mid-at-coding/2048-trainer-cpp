#include "config.hpp"
#include <fstream>
#include <vector>
#include "error.hpp"
std::map<std::string, std::string> config;

void readConfig(){
	Logger logger;
	std::ifstream file;
	file.open("./config.txt");
	std::vector<std::string> data;
	bool readingValue = false;
	std::string currentLine;
	std::string temp;
	if(!file.is_open()){
		logger.log("Failed to open config", Logger::FATAL);
		exit(1);
	}
	while(std::getline(file, currentLine)){
		while (currentLine.find("\r") != std::string::npos) {
			currentLine.erase(currentLine.find("\r"), 1);
		}
		data.push_back(currentLine);
	}
	for(int i = 0; i < data.size(); i++){
		currentLine.clear();
		temp.clear();
		readingValue = false;
		for(int j = 0; j < data[i].size(); j++){
			if(data[i][j] == '='){ // switch to reading value
				readingValue = true;
				continue;
			}
			if(!readingValue) // if we aren't reading the value, we must be reading the key
				currentLine += data[i][j];
			else	// if we are reading the value, read into value
				temp += data[i][j];
		}
		config[currentLine] = temp;
	}
}
