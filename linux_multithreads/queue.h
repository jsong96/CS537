/***********************************************************************
* FILENAME :       queue.h             
*
* DESCRIPTION :
*       This is the header file for queue structure.
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon
* CLASS: CS 537
* TERM: FALL 2020
*
**/

#ifndef QUEUE
#define QUEUE
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "statistics.h"

typedef struct queue
{
  char **words;
  int front;
  int rear;
  int queue_size;
  int dequeue_state;
  int enqueue_state;
  int ret;
  sem_t dequeue_ready;
  sem_t enqueue_ready;

  Statistics *stat;

} Queue;

Queue *createStringQueue(int size);

void EnqueueString(Queue *q, char *string);

char *DequeueString(Queue *q);

void PrintQueueStats(Queue *q);

#endif