/***********************************************************************
* FILENAME :       queue.c             
*
* DESCRIPTION :
*       This is the source file for queue header file
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon
* CLASS: CS 537
* TERM: FALL 2020
*
**/

#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

/*
 * Function: createStringQueue
 * ----------------------------
 *   Return: Queue *q
 *
 *   size: size of new Queue
 *
 *   This function initializes new Queue pointer and returns the pointer.
 */
Queue *createStringQueue(int size)
{
    // Create a queue
    Queue *q = (Queue *)malloc(sizeof(Queue));
    if (q == NULL)
    {
        fprintf(stderr, "Error: failed allocate memory for malloc(queue).\n");
        exit(EXIT_FAILURE);
    }

    // A queue can hold 10 characters
    q->words = (char **)malloc(sizeof(char *) * size);
    if (q->words == NULL)
    {
        fprintf(stderr, "Error: failed allocate memory for malloc(words).\n");
        exit(EXIT_FAILURE);
    }
    // Initialize variables
    q->front = 0;
    q->rear = 0;
    q->queue_size = size;

    // Initialize semaphore dequeue_ready
    if (sem_init(&q->dequeue_ready, 0, 0) != 0)
    {
        fprintf(stderr, "Error: failed initializing semaphore dequeue_ready.\n");
        exit(EXIT_FAILURE);
    }
    // Initialize semaphore enqueue_ready
    if (sem_init(&q->enqueue_ready, 0, size) != 0)
    {
        fprintf(stderr, "Error: failed initializing semaphore enqueue_ready.\n");
        exit(EXIT_FAILURE);
    }

    q->stat = createStatistics();
    if (q->stat == NULL)
    {
        fprintf(stderr, "Error: failed initializing Statistics.\n");
        exit(EXIT_FAILURE);
    }

    return q;
}

/*
 * Function: EnqueueString
 * ----------------------------
 *
 *   q: a queue for a string to enqueue
 *   string: a string to put in the queue
 *
 *   This function enqueue the string passed into the Queue q
 */
void EnqueueString(Queue *q, char *string)
{
    startTimeEnqueue(q->stat);
    if (sem_wait(&q->enqueue_ready) != 0)
    {
        fprintf(stderr, "Error: failed to wait semaphore dequeue_ready.\n");
        exit(EXIT_FAILURE);
    }
    int pos = 0;
    if (q->rear == q->queue_size - 1)
    {
        pos = q->rear;
        q->rear = 0;
    }
    else
    {
        pos = q->rear++;
    }

    countEnqueue(q->stat);

    q->words[pos] = string;

    if (sem_post(&q->dequeue_ready) != 0)
    {
        fprintf(stderr, "Error: failed to post semaphore enqueue_ready.\n");
        exit(EXIT_FAILURE);
    }
    endTimeEnqueue(q->stat);
}

/*
 * Function: DequeueString
 * ----------------------------
 *  
 *   Return: char *removed
 * 
 *   q: a queue for a string to dequeue
 *
 *   This function dequeues a string from Queue q
 */
char *DequeueString(Queue *q)
{
    startTimeDequeue(q->stat);
    if (sem_wait(&q->dequeue_ready) != 0)
    {
        fprintf(stderr, "Error: failed to wait semaphore enqueue_ready.\n");
        exit(EXIT_FAILURE);
    }
    int pos = q->front;
    if (q->front == q->queue_size - 1)
    {
        q->front = 0;
    }
    else
    {
        q->front++;
    }
    char *removed = q->words[pos];

    countDequeue(q->stat);

    if (sem_post(&q->enqueue_ready) != 0)
    {
        fprintf(stderr, "Error: failed to post semaphore dequeue_ready.\n");
        exit(EXIT_FAILURE);
    }
    endTimeDequeue(q->stat);
    return removed;
}

/*
 * Function: PrintQueueStats
 * ----------------------------
 *
 *   q: a queue to print out statistics
 *
 *   This function calls printStatistics function from statistics.c file to
 *   print out statistics for each queue
 */
void PrintQueueStats(Queue *q)
{
    printStatistics(q->stat);
}
