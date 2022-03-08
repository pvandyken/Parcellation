CC = c++  -fopenmp

CFLAGS = -c -Wall -O0  -std=c++17 
SRC = ${wildcard src/*.cpp}
INCLUDE = -I extern/catch2/single_include/catch2
OBJS = ${patsubst src/%.cpp,build/tmp/%.o,${SRC}} main.cpp 

all: main

main:${OBJS} ${wildcard src/*.h}
	$(CC) -std=c++17  ${OBJS} ${INCLUDE} -o main



build/tmp/%.o : src/%.cpp
	mkdir -p ${dir $@}
	${CC}  ${INCLUDE} -o $@ $< -c ${CFLAGS}

clean:
	rm -rf main build/tmp
