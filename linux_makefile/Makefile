CC      = gcc
CFLAGS = -Wall -pedantic
EXE = 537make

537make: main.o textParsing.o specGraph.o createExecute.o specRep.o
	$(CC) $(CFLAGS) -o 537make main.o textParsing.o specGraph.o createExecute.o specRep.o

main.o: main.c textParsing.h specGraph.h specRep.h
	$(CC) $(CFLAGS) -c main.c

textParsing.o: textParsing.h specGraph.h specRep.h textParsing.c 
	$(CC) $(CFLAGS) -c textParsing.c

specGraph.o: specGraph.h specRep.h specGraph.c
	$(CC) $(CFLAGS) -c specGraph.c

createExecute.o: createExecute.h createExecute.c
	$(CC) $(CFLAGS) -c createExecute.c

specRep.o: specRep.h specRep.c
	$(CC) $(CFLAGS) -c specRep.c

clean:
	rm $(EXE) *.o
	


