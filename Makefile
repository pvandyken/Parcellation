CC = g++ -g -std=c++11 #-fopenmp

CFLAGS = -c -Wall -O0
SRC = ${wildcard src/*.cpp}
OBJS = ${patsubst src/%.cpp,build/tmp/%.o,${SRC}} main.cpp 

all: main

main:${OBJS} ${wildcard src/*.h}
	$(CC) ${OBJS}  -o main



build/tmp/%.o : src/%.cpp
	mkdir -p ${dir $@}
	${CC} -o $@ $< -c ${CFLAGS}

clean:
	rm -rf main build/tmp
