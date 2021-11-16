/***********************************************************************
* FILENAME :       statistics.h             
*
* DESCRIPTION :
*       This is the header file for statistics functions
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon
* CLASS: CS 537
* TERM: FALL 2020
*
**/

#ifndef STATSTICS
#define STATISTICS
#include <time.h>

typedef struct statistics
{
    int enqueueCount;
    int dequeueCount;
    double enqueueTime;
    double dequeueTime;
    volatile clock_t enqueue;
    volatile clock_t dequeue;
} Statistics;

Statistics *createStatistics();
void startTimeEnqueue(Statistics *stat);
void endTimeEnqueue(Statistics *stat);
void startTimeDequeue(Statistics *stat);
void endTimeDequeue(Statistics *stat);
void countDequeue(Statistics *stat);
void countEnqueue(Statistics *stat);
void printStatistics(Statistics *stat);
#endif
