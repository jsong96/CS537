/***********************************************************************
* FILENAME :       main_lru.c             
*
* DESCRIPTION :
*       This is a main source file for lru page replacement algorithm
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon
* CLASS: CS 537
* TERM: FALL 2020
*
**/
#include <stdio.h>
#include <stdlib.h>
#include "input.h"
#include "diskQueue.h"
#include "pageAlgorithm.h"

/*
 * Function: main
 * ----------------------------
 *  
 *   Returns: 1
 * 
 *   int argc: number of argument passed
 *   char **argv: list of argument passed
 *
 *   This function parses argument passed and determine size of memory, pagetable
 *     and page. It then reads a file with name that is passed upon and filter out
 *     unique pids. It rereads the file and simulate lru page replacement algorithm
 *     and prints out its stat.
 */
int main(int argc, char **argv)
{
  flags *f = parse_options(argc, argv);
  int pagetable_size = f->memsize / f->pagesize;
  pidList *pid_list;
  pidList *uniquelist;
  DiskQueue *dq;
  if (f->filename != NULL)
  {
    pid_list = get_pidlist(f->filename);
    uniquelist = filter_pidlist(pid_list);
    pid_list = NULL;
    printf("\n------------------ RUNNING %s --------------------------\n", f->filename);
    dq = lru(f->filename, pagetable_size, uniquelist);
    printf("\n------------------ Statistic  -----------------------\n");
    printf("AMU: %f\n", dq->amu);
    printf("ARP: %f\n", dq->arp);
    printf("TMR: %d\n", dq->tmr);
    printf("TPI: %d\n", dq->pageFault);
    printf("RTime: %ld\n", dq->runtime);
  }
  else
  {
    fprintf(stderr, "Error: filename is not provided.\n");
    exit(EXIT_FAILURE);
  }
  // FREE MEMORY
  for (int i = 0; i < uniquelist->size; i++)
  {
    free(uniquelist->list[i]);
  }
  free(uniquelist);
  free(f);
  return 0;
}
