#######################################################################
#
# Makefile for Virtual Disk Simulator
#
# Author: 	Andrew Smythe
#
# Date:		November 16th, 2012
#
# Targets:  run -- build and run the program
#
#######################################################################

# Tools
CC=gcc

run: asn02
	./asn02

asn02: disk.o main.o int.o
	$(CC) -Wall disk.o main.o int.o -o asn02 -lm
	
disk.o: disk.c disk.h block.h
	$(CC) -Wall -c disk.c -o disk.o -lm
	
int.o: int.c int.h
	$(CC) -Wall -c int.c -o int.o -lm
	
main.o: main.c disk.h block.h
	$(CC) -Wall -c main.c -o main.o -lm

clean:
	rm -f *.o asn01

