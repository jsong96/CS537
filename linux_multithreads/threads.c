/***********************************************************************
* FILENAME :       threads.c             
*
* DESCRIPTION :
*       This is the source file for threads header file
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon
* CLASS: CS 537
* TERM: FALL 2020
*
**/

#include "threads.h"
#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/*
 * Function: read_input
 * ----------------------------
 *   Function for reader thread
 *
 *   queues: void pointer for Queue array
 *
 *   This function reads input through stdin and stores line into Queue q1.
 *   It ignores lines that are longer than maximum buffer size 4K.
 *   When finished reading all the input, it enqueue NULL pointer to Queue q1.
 */
void *read_input(void *queues)
{
  Queue **qp = (Queue **)queues;
  const int BUFFER = 50960;
  char c = '\0';
  int idx = 0;
  int ignoreline = 0;
  int newline = 1;
  char *linebuffer = NULL;

  while ((c = getc(stdin)) != EOF)
  {
    if (newline)
    {
      // new memory for new line buffer
      linebuffer = calloc(BUFFER, sizeof(char));
      if (linebuffer == NULL)
      {
        fprintf(stderr, "Error: cannot allocate memory for linebuffer.\n");
        exit(EXIT_FAILURE);
      }
      newline = 0;
      ignoreline = 0;
    }

    // enqueue line buffer and reset for each line
    if (c == '\n')
    {
      idx = 0;
      newline = 1;
      if (linebuffer != NULL)
      {
        if (!ignoreline)
        {
          EnqueueString(qp[0], linebuffer);
          // reset buffer
          linebuffer = NULL;
        }
        else
        {
          // ignore lines should be set free to save memory
          free(linebuffer);
          // reset buffer
          linebuffer = NULL;
        }
      }
    }
    else
    {
      // ignore line that are larger than buffer size
      if (ignoreline != 1)
      {
        if (idx <= BUFFER)
        {
          linebuffer[idx] = c;
          idx++;
        }
        else
        {
          fprintf(stderr, "Error: Input line exceeded the buffer size of 4K.\n");
          ignoreline = 1;
        }
      }
    }
  }

  if (linebuffer != NULL)
  {
    EnqueueString(qp[0], linebuffer);
    linebuffer = NULL;
  }

  // NULL enqueue for termination signal
  EnqueueString(qp[0], NULL);
  pthread_exit(NULL);
}

/*
 * Function: space_to_asterisk
 * ----------------------------
 *   Function for munch_one thread
 *
 *   queues: void pointer for Queue array
 *
 *   This function dequeues lines from Queue q1 and converts all the space
 *   characters to asterisk characters and enqueue them back to Queue q2. It
 *   terminates when it meets NULL pointer in Queue q1.
 */
void *space_to_asterisk(void *queues)
{
  Queue **qp = (Queue **)queues;
  char *line = NULL;
  char *space_found = NULL;
  const char asterisk = '*';
  const char space = ' ';
  while (1)
  {
    line = DequeueString(qp[0]);
    if (line == NULL)
    {
      break;
    }
    while ((space_found = index(line, space)) != NULL)
    {
      *(space_found) = asterisk;
    }
    EnqueueString(qp[1], line);
  }
  // NULL enqueue for termination signal
  EnqueueString(qp[1], NULL);
  pthread_exit(NULL);
}

/*
 * Function: lower_to_upper
 * ----------------------------
 *   Function for munch_two thread
 *
 *   queues: void pointer for Queue array
 *
 *   This function dequeues lines from Queue q2 and converts all the lowercase
 *   letter to uppercase letter and enqueue them back to Queue q3. It
 *   terminates when it meets NULL pointer in Queue q2.
 */
void *lower_to_upper(void *queues)
{
  Queue **qp = (Queue **)queues;
  char *line = NULL;
  while (1)
  {
    line = DequeueString(qp[1]);
    // if the line is null, exit
    if (line == NULL)
    {
      break;
    }

    //strlen does not count for \0
    int length = strlen(line) + 1;
    for (int i = 0; i < length; i++)
    {
      if (islower(line[i]))
      {
        // convert each lowercase to uppercase
        line[i] = toupper(line[i]);
      }
    }
    EnqueueString(qp[2], line);
  }
  // NULL enqueue for termination signal
  EnqueueString(qp[2], NULL);
  pthread_exit(NULL);
}

/*
 * Function: write_to_console
 * ----------------------------
 *   Function for writer thread
 *
 *   queues: void pointer for Queue array
 *
 *   This function dequeues lines from Queue q3 and prints them out to the console. 
 *   It terminates when it meets NULL pointer in q3.
 */
void *write_to_console(void *queues)
{
  Queue **qp = (Queue **)queues;
  char *line = NULL;
  int count = 0;
  while (1)
  {
    // dequeue strings until it meets NULL
    line = DequeueString(qp[2]);
    if (line != NULL)
    {
      printf("%s\n", line);
      free(line);
      line = NULL;
      count++;
    }
    else
    {

      printf("The number of strings processed to stdout: %d\n", count);
      break;
    }
  }

  pthread_exit(NULL);
}