#include "formatting.hpp"

std::string colorString(std::string msg, Color color, bool background){
	RGB rgb = color.getRGB().value(); // possible segfault
	return (std::string("\e[") + (background ? "4" : "3") + "8;2;" + std::to_string(rgb.r) + ";" + std::to_string(rgb.g) + ";" + std::to_string(rgb.b) + "m" + msg + "\e[0m");
}
