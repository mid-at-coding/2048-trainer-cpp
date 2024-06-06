#include <iostream>
#include "../include/error.hpp"
#include "../include/formatting.hpp"

// this file should really be renamed

Logger::severity Logger::outputThreshold = Logger::INFO;

std::string Logger::levelToStr(int sev){
	switch(sev){
		case DEBUG:
			return "[DEBUG] ";
		case INFO:
			return "[INFO] ";
		case WARN:
			return colorString("[WARN] ", Color(RGB{255, 255, 0}));
		case NONFATAL:
			return colorString("[NONFATAL] ", Color(RGB{255, 75, 0}));
		case FATAL: 
			return colorString("[FATAL] ",Color(RGB{255,0,0}));
		case NOTAG:
			return "";
		default: // ??
			return "[FATAL] ";
	}
}
void Logger::log(std::string msg, severity sev, int indentation){
	if(sev < outputThreshold)
		return;
	std::cout <<levelToStr(sev);
	for(int i = 0; i < indentation; i++)
		std::cout << "  ";
	std::cout << msg << std::endl;
}
