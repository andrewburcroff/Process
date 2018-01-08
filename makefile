CC = gcc
CFlAGS = -g -Wall -pedantic -std=gnu99 -pthread

all: main

main: process.c
				${CC} ${CFlAGS} process.c

clean:
	rm -f process process.o
