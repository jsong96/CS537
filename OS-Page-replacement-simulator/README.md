# Simulating Page Replacement Algorithms

### Authors: HyukJoon Yang, Jiwon Song

executable names: 537pfsim-fifo, 537pfsim-lru, 537pfsim-clock

tracefiles: smallmix.addrtrace, 4000.addrtrace, bigmix.addrtrace, 12million.addrtrace

makefile
 - individual rule for each executives
 
This is a page replacement simulation program. Each executable simulates different policy of page replacement algorithms.
The policies are First-In-First-Out, Least Recently Used, and Clock.

## How to run
First, type **make** and compile all the files.
Then, run simulators by
 (filename) (options) (filename)
 ex. 537pfsim-fifo -p 4096 -m 4 4000.addrtrace
 
### Options
 -p : a number of bytes, determines the size of each page. Must be power of 2. The default is 4096.
 -m : a number of MB, determines the size of memory. The default is 1MB.

### Modules
input.c 
 - parse options
 - parameter checking
 - read each line in a trace file (assume all references are read)
 
pagetable.c 
 - initializes a page table  
 - initializes an inverted page table
 
pagealgorithm.c
 - contains methods of fifo, lru and clock
 
 diskQueue.c
 - represents Disk I/O where blocked process waits until it is freed
 

### Algorithm flow
Reading each line of tracefile....
 parse and check if the pid is blocked:
  IF pid is not blocked:
   CHECK PAGE FAULT:
    IF hit -> move on to next line
    IF miss and DiskI/O is not full
      INSERT page into the page table
    IF miss and DiskI/O is full
      RUN replacement on page
  ELSE
   CHECK space for inverted page table
    IF there is a space:
     dequeue Disk I/O
     load data into memory
     update a queue that holds PPN
    ELSE:
     pull out PPN to replace (depends on replacement policy)
     replace PPN with new PPN from the page table
     load data into memory
     update a queue that holds PPN
 
### Results
 At the end of each simulator, it will print out statistic results.
 - Average Memory Utilization (AMU)L an average of how many pages frames are occupied (per clock tick)
 - Average Runable Processes (ARP): an average of the number of processes that are running (per clock tick)
 - Total Memory References (TMR): a count of the total number of memory references in the trace file.
 - Total Page Ins (TPI): total number of page faults


