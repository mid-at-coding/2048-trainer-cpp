#include "error.hpp"
#include "maps.hpp"
#include "config.hpp"
#include "move.hpp"
#include "movedir.hpp"
#include "satisfied.hpp"
#include "solve.hpp"

int main(){
	for(int i = std::stoi(config["s0"]); i > std::stoi(config["s1"]); i -= 2){
		solve(i);
	}
}
