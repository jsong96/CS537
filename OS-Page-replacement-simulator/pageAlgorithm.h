/***********************************************************************
* FILENAME :       pageAlgorithm.h             
*
* DESCRIPTION :
*       This is the header file for pageAlgorithm.c
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon
* CLASS: CS 537
* TERM: FALL 2020
*
**/

#ifndef PAGE_ALGORITHM
#define PAGE_ALGORITHM
#include "diskQueue.h"
#include "pagetable.h"

parseNode *getPidandVpn(char *line);

int find_idx(pidList *pidlist, long int pid);

DiskQueue *fifo(char *filename, int pagetable_size, pidList *uniquelist);

DiskQueue *lru(char *filename, int pagetable_size, pidList *pidlist);

DiskQueue *clock_replacement(char *filename, int pagetable_size, pidList *pidlist);

#endif
