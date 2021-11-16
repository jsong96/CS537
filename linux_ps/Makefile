# Author: Jiwon Song, HyukJoon Yang
# Class: CS 537 
# Term: FALL 2020

CXX = gcc
CXXFLAG = -Wall -pedantic

537ps: main.o psfind.o
	$(CXX) $(CXXFLAG) -o 537ps main.o psfind.o

main.o: main.c
	$(CC) $(CXXFLAG) -c main.c

psfind.o: psfind.c psfind.h
	$(CC) $(CXXFLAG) -c psfind.c

clean:
	-rm main.o
	-rm psfind.o
	-rm 537ps
