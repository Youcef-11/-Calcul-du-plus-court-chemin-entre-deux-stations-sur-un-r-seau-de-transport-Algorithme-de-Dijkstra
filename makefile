CXXFLAGS2 =-Wall -Wextra -Werror -pedantic -pedantic-errors -O3 -g -std=c++11 

all: main

main: main.cpp src/Generic_heritage.cpp src/Grade.o 
	g++ -o bin/executable main.cpp src/Generic_heritage.cpp src/Grade.o $(CXXFLAGS) 
