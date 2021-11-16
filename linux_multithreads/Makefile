CC = gcc
WARNING_FLAGS = -Wall -Wextra
OPTS = -pthread
EXE = prodcom
SCAN_BUILD_DIR = scan-build-out

all: main.o threads.o statistics.o queue.o
	$(CC) $(OPTS) -o $(EXE) main.o threads.o statistics.o queue.o

main.o: main.c queue.h threads.h
	$(CC) $(OPTS) $(WARNING_FLAGS) -c main.c

queue.o: queue.c queue.h statistics.h
	$(CC) $(OPTS) $(WARNING_FLAGS) -c queue.c

threads.o: threads.c queue.h 
	$(CC) $(OPTS) $(WARNING_FLAGS) -c threads.c

statistics.o: statistics.c statistics.h
	$(CC) $(OPTS) $(WARNING_FLAGS) -c statistics.c

clean:
	rm -f $(EXE) *.o
	rm -rf $(SCAN_BUILD_DIR)

#
# Run the Clang Static Analyzer
#
scan-build: clean
	scan-build -o $(SCAN_BUILD_DIR) make

#
# View the one scan available using firefox
#
scan-view: scan-build
	firefox -new-window $(SCAN_BUILD_DIR)/*/index.html 
