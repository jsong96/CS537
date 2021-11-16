
/***********************************************************************
* FILENAME :       diskQueue.c             
*
* DESCRIPTION :
*       
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon
* CLASS: CS 537
* TERM: FALL 2020
*
**/

#include "diskQueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <search.h>

/*
 * Function: createDiskQueue
 * ----------------------------
 *  
 *   Returns: DiskQueue *queue
 * 
 *   int size :  Size for DiskQueue
 *
 *   This function initializes DiskQueue
 */
DiskQueue *createDiskQueue(int size)
{
  PPNQueue *ppnqueue = malloc(sizeof(PPNQueue));
  if (ppnqueue == NULL)
  {
    fprintf(stderr, "Error createDiskQueue: malloc PPNQueue\n");
    exit(EXIT_FAILURE);
  }
  ppnqueue->root = NULL;
  ppnqueue->tail = NULL;

  DiskQueue *queue = malloc(sizeof(DiskQueue));
  if (queue == NULL)
  {
    fprintf(stderr, "Error createDiskQueue: malloc DiskQueue\n");
    exit(EXIT_FAILURE);
  }
  queue->root = NULL;
  queue->tail = NULL;
  queue->pageFault = 0;
  queue->amu = 0;
  queue->arp = 0;
  queue->tmr = 0;
  queue->tpi = 0;
  queue->runtime = 0;
  queue->ppnqueue = ppnqueue;
  queue->count = 0;
  queue->size = size;
  return queue;
}

/*
 * Function: enqueuePPNQueue
 * ----------------------------
 * 
 *   DiskQueue *queue : DiskQueue that stores PPNQueue 
 *   struct PPNNode *ppnnode: PPNNode to be stored in PPNQueue
 *   
 *   This function inserts PPNNode into the PPNQueue in DiskQueue
 */
void enqueuePPNQueue(DiskQueue *queue, struct PPNNode *ppnnode)
{
  if (queue->ppnqueue->root == NULL)
  {
    queue->ppnqueue->root = ppnnode;
  }
  else
  {
    ppnnode->prev = queue->ppnqueue->tail;
    queue->ppnqueue->tail->next = ppnnode;
  }
  ppnnode->next = NULL;
  queue->ppnqueue->tail = ppnnode;
}

/*
 * Function: dequeuePPNQueue
 * ----------------------------
 *  
 *   Returns: struct PPNNode *ppnnode
 * 
 *   DiskQueue *queue : DiskQueue that stores PPNQueue
 *   InvertTree *tree : Inverted page table 
 *
 *   This functions dequeues a PPNNode to be replaced from memory
 */
struct PPNNode *dequeuePPNQueue(DiskQueue *queue, InvertTree *inverttree)
{
  //void *result = NULL;
  struct PPNNode *currNode = queue->ppnqueue->root;
  struct PPNNode *temp = NULL;
  while (currNode != NULL)
  {
    if (currNode->ppn == 0)
    {
      queue->ppnqueue->root = currNode->next;
      currNode->next->prev = NULL;
      temp = currNode->next;
      free(currNode);
      currNode = temp;
    }
    else
    {
      queue->ppnqueue->root = currNode->next;
      currNode->next->prev = NULL;
      return currNode;
    }
  }
  return NULL;
}

/*
 * Function: replacePPNQueue
 * ----------------------------
 *  
 *   DiskQueue *queue : DiskQueue that stores PPNQueue
 *   struct PPNNode *ppnnode : PPNNode to be replaced to the tail
 *
 *   This function is used for tracking nodes for LRU replacement.
 *   It places ppnnode at the tail of PPNQueue
 */
void replacePPNQueue(DiskQueue *queue, struct PPNNode *ppnnode)
{
  // Only PPNNode in the queue
  if (ppnnode->prev == NULL && ppnnode->next == NULL)
  {
    return;
  }
  // Root of PPNQueue
  else if (ppnnode->prev == NULL)
  {
    queue->ppnqueue->root = ppnnode->next;
    ppnnode->next->prev = NULL;
    ppnnode->prev = queue->ppnqueue->tail;
    queue->ppnqueue->tail->next = ppnnode;
    ppnnode->next = NULL;
    queue->ppnqueue->tail = ppnnode;

  } 
  // Tail of PPNQueue
  else if (ppnnode->next == NULL)
  {
    return;
  }

  // Middle of PPNQueue
  else 
  {
    ppnnode->prev->next = ppnnode->next;
    ppnnode->next->prev = ppnnode->prev;
    
    ppnnode->next = NULL;
    ppnnode->prev = queue->ppnqueue->tail;
    queue->ppnqueue->tail->next = ppnnode;

    queue->ppnqueue->tail = ppnnode;

  }
}

/*
 * Function: enqueueDiskQueue
 * ----------------------------
 *  
 *   DiskQueue *queue : DiskQueue
 *   struct PPNNode *ppnnode : PPNNode to be stored in DiskQueue
 *   long int pos : Location of trace file for the PPNNode
 *   
 *   This function enqueue PPNNode in the DiskQueue
 */
void enqueueDiskQueue(DiskQueue *queue, struct PPNNode *ppnnode, long int pos)
{
  queue->pageFault++;
  const int two_ms = 2000000;
  DiskQueueNode *newNode = malloc(sizeof(DiskQueueNode));
  if (newNode == NULL)
  {
    fprintf(stderr, "Error enequeueDiskQueue: malloc newNode\n");
    exit(EXIT_FAILURE);
  }
  newNode->ppnnode = ppnnode;
  newNode->pos = pos;
  newNode->finishTime = queue->runtime + two_ms;
  newNode->next = NULL;

  DiskQueueNode *lastNode = queue->tail;
  queue->count++;

  if (lastNode == NULL)
  {
    queue->root = newNode;
    queue->tail = newNode;
    return;
  }

  queue->tail->next = newNode;

  newNode->finishTime = queue->tail->finishTime + two_ms;
  queue->tail = newNode;
}

/*
 * Function: dequeueDiskQueue
 * ----------------------------
 *  
 *   Returns: DiskQueueNode *node
 * 
 *   DiskQueue *queue : DiskQueue
 *   InvertTree *inverttree : Inverted page table
 *
 *   This functions dequeues DiskQueueNode from the DiskQueue.
 */
DiskQueueNode *dequeueDiskQueue(DiskQueue *queue, InvertTree *inverttree)
{
  DiskQueueNode *node = queue->root;
  if (node == NULL)
  {
    fprintf(stderr, "Error: diskqueue is empty.\n");
    exit(EXIT_FAILURE);
  }
  if (node->finishTime > queue->runtime)
  {
    queue->amu += (node->finishTime - queue->runtime) * inverttree->count;
    queue->runtime = node->finishTime;
  }

  if (queue->count == 1)
  {
    queue->root = NULL;
    queue->tail = NULL;
  }
  else
  {
    queue->root = queue->root->next;
  }

  queue->count--;

  return node;
}

/*
 * Function: enqueueClockQueue
 * ----------------------------
 *  
 *   DiskQueue *dq : DiskQueue that stores PPNQueue
 *   struct PPNNode *ppnnode : PPNnode to be enqueued
 *   
 *   This function is used to enqueue PPNNode for clock replacement.
 */
void enqueueClockQueue(DiskQueue *dq, struct PPNNode *ppnnode)
{
  if (dq->ppnqueue->root == NULL)
  {
    ppnnode->next = ppnnode;
    ppnnode->prev = ppnnode;
    dq->ppnqueue->root = ppnnode;
    dq->ppnqueue->tail = ppnnode;
    return;
  }

  dq->ppnqueue->root->prev->next = ppnnode;
  ppnnode->prev = dq->ppnqueue->root->prev;

  dq->ppnqueue->root->prev = ppnnode;
  ppnnode->next = dq->ppnqueue->root;


}

/*
 * Function: dequeueClockQueue
 * ----------------------------
 *  
 *   Returns: struct PPNNode *currNode or NULL
 * 
 *   DiskQueue *dq : DiskQueue that stores PPNQueue
 *
 *   This function returns PPNNode to be deleted from memory based
 *   on clock replacement algorithm.
 */
struct PPNNode *dequeueClockQueue(DiskQueue *dq)
{
  struct PPNNode *currNode = dq->ppnqueue->root;
  struct PPNNode *nextNode = NULL;

  while (currNode != NULL)
  {
    if (currNode->ppn == 0)
    {
      currNode->prev->next = currNode->next;
      currNode->next->prev = currNode->prev;
      nextNode = currNode->next;

      free(currNode);
      currNode = nextNode;
    }
    else if(currNode->check == 1)
    {
      currNode->check--;
      currNode = currNode->next;
    }
    else
    {
      currNode->prev->next = currNode->next;
      currNode->next->prev = currNode->prev;
      dq->ppnqueue->root = currNode->next;
      return currNode;
    }
  }
  return NULL;
}
