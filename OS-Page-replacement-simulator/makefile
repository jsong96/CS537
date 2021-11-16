CC     = gcc
CFLAGS = -Wall -pedantic

all: 537pfsim-clock 537pfsim-fifo  537pfsim-lru  

537pfsim-fifo: main_fifo.o input.o pageAlgorithm.o pagetable.o diskQueue.o
	$(CC) $(CFLAGS) -o 537pfsim-fifo input.o pageAlgorithm.o main_fifo.o pagetable.o diskQueue.o

537pfsim-lru: main_lru.o input.o pageAlgorithm.o pagetable.o diskQueue.o
	$(CC) $(CFLAGS) -o 537pfsim-lru input.o pageAlgorithm.o main_lru.o pagetable.o diskQueue.o

537pfsim-clock: main_clock.o input.o pageAlgorithm.o pagetable.o diskQueue.o
	$(CC) $(CFLAGS) -o 537pfsim-clock input.o pageAlgorithm.o main_clock.o pagetable.o diskQueue.o

main_fifo.o: main_fifo.c pageAlgorithm.h input.h diskQueue.h
	$(CC) $(CFLAGS) -c main_fifo.c

main_lru.o: main_lru.c pageAlgorithm.h input.h diskQueue.h
	$(CC) $(CFLAGS) -c main_lru.c

main_clock.o: main_clock.c pageAlgorithm.h input.h diskQueue.h
	$(CC) $(CFLAGS) -c main_clock.c

pageAlgorithm.o: pageAlgorithm.c pageAlgorithm.h pagetable.h input.h diskQueue.h
	$(CC) $(CFLAGS) -c pageAlgorithm.c

pagetable.o: pagetable.c pagetable.h diskQueue.h
	$(CC) $(CFLAGS) -c pagetable.c

diskQueue.o: diskQueue.c diskQueue.h pagetable.h	
	$(CC) $(CFLAGS) -c diskQueue.c

input.o: input.c input.h
	$(CC) $(CFLAGS) -c input.c

clean:
	-rm *.o
	-rm 537pfsim-fifo
	-rm 537pfsim-lru
	-rm 537pfsim-clock
