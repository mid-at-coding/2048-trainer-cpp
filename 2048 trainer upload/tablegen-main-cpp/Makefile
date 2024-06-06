CC=g++
CCFLAGS=-Wall -std=gnu++20 -O3 -ltbb -fopenmp
EXEC_FILE=tablegen
FILES=$(addsuffix .o,$(addprefix build/,$(notdir $(basename $(wildcard src/*.cpp)))))
.PHONY: all

all: $(FILES) tablegen

build/%.o: src/%.cpp 
	$(CC) $< $(CCFLAGS) -c -o $@ 

tablegen:
	$(CC) $(wildcard build/*.o) $(CCFLAGS) -o tablegen
