CC = g++ -std=c++11 -fopenmp

CFLAGS = -c -Wall -O3

all: main

main: main.o intersection.o felipeTools.o
	$(CC) main.o intersection.o felipeTools.o -o main

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

intersection.o: intersection.cpp
	$(CC) $(CFLAGS) intersection.cpp

felipeTools.o: felipeTools.cpp
	$(CC) $(CFLAGS) felipeTools.cpp

clean:
	rm -rf *o main intersection felipeTools
