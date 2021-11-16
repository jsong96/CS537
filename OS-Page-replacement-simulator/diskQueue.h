/***********************************************************************
* FILENAME :       diskQueue.h             
*
* DESCRIPTION :
*       This is the header file for diskQueue.c
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon
* CLASS: CS 537
* TERM: FALL 2020
*
**/

#ifndef DISK_QUEUE
#define DISK_QUEUE
#include "pagetable.h"

typedef struct ppnqueue
{
  struct PPNNode *root;
  struct PPNNode *tail;

} PPNQueue;

typedef struct diskqueuenode
{
  struct PPNNode *ppnnode;
  long int pos;
  int startTime;
  long int finishTime;
  struct diskqueuenode *next;
} DiskQueueNode;

typedef struct diskqueue
{
  DiskQueueNode *root;
  DiskQueueNode *tail;
  int pageFault;
  PPNQueue *ppnqueue;
  double amu;
  double arp;
  int tmr;
  int tpi;
  long int runtime;
  int size;
  int count;

} DiskQueue;

DiskQueue *createDiskQueue(int size);

void enqueuePPNQueue(DiskQueue *queue, struct PPNNode *ppnnode);

struct PPNNode *dequeuePPNQueue(DiskQueue *queue, InvertTree *inverttree);

void replacePPNQueue(DiskQueue *queue, struct PPNNode *ppnnode);

void enqueueDiskQueue(DiskQueue *queue, struct PPNNode *newNode, long int pos);

DiskQueueNode *dequeueDiskQueue(DiskQueue *queue, InvertTree *inverttree);

void enqueueClockQueue(DiskQueue *dq, struct PPNNode *ppnnode);

struct PPNNode *dequeueClockQueue(DiskQueue *dq);


#endif

