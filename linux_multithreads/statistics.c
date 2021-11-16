/***********************************************************************
* FILENAME :       statistics.c             
*
* DESCRIPTION :
*       This is the source file for statistics header file
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon
* CLASS: CS 537
* TERM: FALL 2020
*
**/

#include "statistics.h"
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
/*
 * Function: createStatistics
 * ----------------------------
 *   Return Statistics struct pointer 
 *
 *
 *   This function creates Statistics struct 
 */
Statistics *createStatistics()
{
    Statistics *stat = (Statistics *)malloc(sizeof(Statistics));
    if (stat == NULL)
    {
        fprintf(stderr, "Error: Failed allocate memory of malloc(Statistics)");
        exit(EXIT_FAILURE);
    }

    stat->enqueueCount = 0;
    stat->dequeueCount = 0;
    stat->enqueueTime = 0;
    stat->dequeueTime = 0;
    return stat;
}

/*
 * Function: startTimeEnqueue
 * ----------------------------
 *   
 *   stat: Statistics struct stored in a queue
 *
 *   This function records the clock when an Enqueue process of a 
 *   queue for a string is initiatied 
 */
void startTimeEnqueue(Statistics *stat)
{
    stat->enqueue = clock();
}

/*
 * Function: endTimeEnqueue
 * ----------------------------
 *   
 *   stat: Statistics struct stored in a queue
 *
 *   This function records the clock when an Enqueue process of a 
 *   queue for a string is finished. Then calculates the elapsed time
 *   for the whole Enqueue process of a string.
 */
void endTimeEnqueue(Statistics *stat)
{
    stat->enqueue = clock() - stat->enqueue;
    stat->enqueueTime += (float)((double)stat->enqueue) / CLOCKS_PER_SEC;
}

/*
 * Function: countEnqueue
 * ----------------------------
 *   
 *   stat: Statistics struct stored in a queue
 *
 *   This function increments the enqueueCount of Statistics of a 
 *   queue when a new string is enqueued in the queue
 */
void countEnqueue(Statistics *stat)
{
    stat->enqueueCount++;
}

/*
 * Function: startTimeDequeue
 * ----------------------------
 *   
 *   stat: Statistics struct stored in a queue
 *
 *   This function records the clock when a Dequeue process of a queue 
 *   for a string is initiatied 
 */
void startTimeDequeue(Statistics *stat)
{
    stat->dequeue = clock();
}

/*
 * Function: endTimeDequeue
 * ----------------------------
 *   
 *   stat: Statistics struct stored in a queue
 *
 *   This function records the clock when a Dequeue process of a queue 
 *   for a string is finished. Then calculates the elapsed time for 
 *   the whole dequeue process of a string.
 */
void endTimeDequeue(Statistics *stat)
{
    stat->dequeue = clock() - stat->dequeue;
    stat->dequeueTime += (float)((double)stat->dequeue) / CLOCKS_PER_SEC;
}

/*
 * Function: countDequeue
 * ----------------------------
 *   
 *   stat: Statistics struct stored in a queue
 *
 *   This function increments the dequeueCount of Statistics of a 
 *   queue when a new string is dequeued from the queue
 */
void countDequeue(Statistics *stat)
{
    stat->dequeueCount++;
}

/*
 * Function: printStatistics
 * ----------------------------
 *   
 *   stat: Statistics struct stored in a queue
 *
 *   This function is called from the queue to print out the
 *   statistics for each queue.
 */
void printStatistics(Statistics *stat)
{
    fprintf(stderr, "Enqueue Count: %d\n", stat->enqueueCount);
    fprintf(stderr, "Enqueue Time: %f\n", stat->enqueueTime);
    fprintf(stderr, "Dequeue Count: %d\n", stat->dequeueCount);
    fprintf(stderr, "Dequeue Time: %f\n", stat->dequeueTime);
}
