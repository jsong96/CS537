/***********************************************************************
* FILENAME :       main.c             
*
* DESCRIPTION :
*       This is the main source file that executes thread functions
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
#include "threads.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/*
 * Function: main
 * ----------------------------
 *   Return 0
 *
 *   This function synchronously runs four threads: reader, writer, munch_one,
 *   munch_two. The reader thread reads each line in input and passes it to
 *   munch_one. The munch_one thread converts all the space characters into
 *   asterisks character and passes it to munch_two. The munch_two thread
 *   converts all the lowercase letters to uppercase letters and passes it to
 *   writer. The writer thread reads each line and prints out.
 */
int main()
{
  // For test purpose, queuesize is set to 10
  const int queueSize = 10;

  /*** Queue initialization ***/
  Queue *q1 = createStringQueue(queueSize);
  if (q1 == NULL)
  {
    fprintf(stderr, "Error: cannot allocate memory for q1.\n");
    exit(EXIT_FAILURE);
  }
  Queue *q2 = createStringQueue(queueSize);
  if (q2 == NULL)
  {
    fprintf(stderr, "Error: cannot allocate memory for q2.\n");
    exit(EXIT_FAILURE);
  }
  Queue *q3 = createStringQueue(queueSize);
  if (q3 == NULL)
  {
    fprintf(stderr, "Error: cannot allocate memory for q3.\n");
    exit(EXIT_FAILURE);
  }

  // put Queues into a void pointer to pass to each thread
  void *queues[3] = {q1, q2, q3};

  /*** thread initialization ***/
  pthread_t reader;
  pthread_t munch_one;
  pthread_t munch_two;
  pthread_t writer;

  if (pthread_create(&reader, NULL, read_input, queues) != 0)
  {
    fprintf(stderr, "Error: failed to create the reader thread.\n");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&munch_one, NULL, space_to_asterisk, queues) != 0)
  {
    fprintf(stderr, "Error: failed to create the munch_one thread.\n");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&munch_two, NULL, lower_to_upper, queues) != 0)
  {
    fprintf(stderr, "Error: failed to create the munch_two thread.\n");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&writer, NULL, write_to_console, queues) != 0)
  {
    fprintf(stderr, "Error: failed to create the writer thread.\n");
    exit(EXIT_FAILURE);
  }

  /*** Start each thread ***/
  if (pthread_join(reader, NULL) != 0)
  {
    fprintf(stderr, "Error: failed to join the reader thread.\n");
    exit(EXIT_FAILURE);
  }

  if (pthread_join(munch_one, NULL) != 0)
  {
    fprintf(stderr, "Error: failed to join the munch_one thread.\n");
    exit(EXIT_FAILURE);
  }

  if (pthread_join(munch_two, NULL) != 0)
  {
    fprintf(stderr, "Error: failed to join the munch_two thread.\n");
    exit(EXIT_FAILURE);
  }

  if (pthread_join(writer, NULL) != 0)
  {
    fprintf(stderr, "Error: failed to join the writer thread.\n");
    exit(EXIT_FAILURE);
  }

  /*** prints out statistics for each Queue ***/
  Queue **qp = (Queue **)queues;
  for (int i = 0; i < 3; i++)
  {
    printf("Queue %d\n", i+1);
    PrintQueueStats(qp[i]);
    printf("\n");
  }

  /*** free allocated memory for each queue ***/
  for (int i = 0; i < 3; i++)
  {
    sem_destroy(&qp[i]->enqueue_ready);
    sem_destroy(&qp[i]->dequeue_ready);
    free(qp[i]->words);
    free(qp[i]->stat);
    free(qp[i]);
  }

  return 0;
}