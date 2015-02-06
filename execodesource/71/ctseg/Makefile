FLAGS=-std=c++98 -Wall -Wextra 
CXX=g++

all: main.o
	${CXX} main.o -o ctseg 

main.o: src/main.cpp
	${CXX} ${FLAGS} -O3 -c src/main.cpp -I.
