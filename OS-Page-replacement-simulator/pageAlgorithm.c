/***********************************************************************
* FILENAME :       pageAlgorithm.c             
*
* DESCRIPTION :
*       This is the source file for pageAlgorithm.h
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon
* CLASS: CS 537
* TERM: FALL 2020
*
**/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <stddef.h>
#include "pagetable.h"
#include "input.h"
#include "diskQueue.h"
#include "pageAlgorithm.h"

/*
 * Function: find_idx
 * ----------------------------
 *  
 *   Returns: index of pid
 * 
 *   pidList *pidlist : a list to iterate
 *   long int pid : a target pid to find
 *
 *   This function simply iterates and returns an index of 
 *     passed process ID
 */
int find_idx(pidList *pidlist, long int pid)
{
  for (int i = 0; i < pidlist->size; i++)
  {
    if (pidlist->list[i]->pid == pid)
    {
      return i;
    }
  }
  return -1;
}

/*
 * Function: getPidandVpn
 * ----------------------------
 *  
 *   Returns: parseNode *
 * 
 *   char *line : a line to parse
 *
 *   This function simply parses a line and returns
 *     a parseNode struct with pid and vpn
 */
parseNode *getPidandVpn(char *line)
{
  parseNode *tmp = malloc(sizeof(parseNode));
  if (tmp == NULL)
  {
    fprintf(stderr, "Error: failed to allocate memory.\n");
    exit(EXIT_FAILURE);
  }
  int size = strlen(line) + 1;
  char *token = NULL;
  unsigned long int pid = 0;
  unsigned long int vpn = 0;
  char buff[size];
  for (int i = 0; i < size; i++)
  {
    buff[i] = line[i];
  }
  token = strtok(buff, " ");
  pid = atoi(token);
  token = strtok(NULL, " ");
  vpn = atoi(token);

  tmp->pid = pid;
  tmp->vpn = vpn;
  return tmp;
}

/*
 * Function: fifo
 * ----------------------------
 *  
 *   Returns: DiskQueue *dq
 * 
 *   char *filename : a trace file to read
 *   int pagetable_size : size of pagetable
 *   pidList *pidlist: list of pids and their finishing reference
 *
 *   This function reads the trace file and executes fifo page replacement
 *   policy and returns DiskQueue object which contains the statistic results
 */
DiskQueue *fifo(char *filename, int pagetable_size, pidList *pidlist)
{
  FILE *fp;
  fp = fopen(filename, "r");
  if (fp == NULL)
  {
    fprintf(stderr, "Error: failed to open the file.\n");
    exit(EXIT_FAILURE);
  }

  /** INITIALIZATION **/
  DiskQueue *dq = createDiskQueue(pidlist->size);
  InvertTree *inverted_pagetable = createInvertTree(pagetable_size);
  ProcessTree *pagetable_tree = createProcTree(pidlist->size);
  for (int i = 0; i < pidlist->size; i++)
  {
    createPageTables(pagetable_tree, i, pidlist->list[i]->pid);
  }

  /** LINE READING VARIABLE **/
  char *line = NULL;
  size_t line_len;
  long int pid = 0;
  long int vpn = 0;
  long int cursor = 0;

  /** OTHER VARIABLE**/
  int faultflag = 0;
  DiskQueueNode *tmp = NULL;
  parseNode *line_parse = NULL;
  pidNode **list = pidlist->list;
  int tmp_invCount = 0;

  /** CURRENTLY WORKING PID **/
  int pididx = 0;
  int total_pid_finished = 0;
  int numprocess = pidlist->size;

  // set starting position
  cursor = list[pididx]->start_reference;

  while (1)
  {
    // stop when all processes are done
    if (total_pid_finished == numprocess)
    {
      break;
    }
    fseek(fp, cursor, SEEK_SET);

    // skip the finished process
    if (list[pididx]->done == 1)
    {
      pididx++;
      if (pididx == numprocess)
      {
        pididx = pididx % numprocess;
      }
      cursor = list[pididx]->curpos;
    }
    else
    {
      // record the starting position of the line
      cursor = ftell(fp);
      while (getline(&line, &line_len, fp) != EOF)
      {
        line_parse = getPidandVpn(line);
        pid = line_parse->pid;
        vpn = line_parse->vpn;
        free(line_parse);

        // only deal with the currently working process
        if (pid == list[pididx]->pid)
        {
          dq->runtime++;
          dq->arp += dq->size - dq->count;

          /** Case for blocked Process **/
          if (list[pididx]->block == 1)
          {
            tmp = dequeueDiskQueue(dq, inverted_pagetable);
            
            /** Case for full inverted page table **/
            if (inverted_pagetable->size == inverted_pagetable->count)
            {
              struct PPNNode *oldNode = dequeuePPNQueue(dq, inverted_pagetable);
              deleteTable(pagetable_tree, inverted_pagetable, oldNode);
              insertTable(pagetable_tree, inverted_pagetable, tmp->ppnnode);
              dq->amu += inverted_pagetable->count;
              enqueuePPNQueue(dq, tmp->ppnnode);
              list[pididx]->block = 0;
              dq->arp += 1;
              cursor = tmp->pos;
              break;
            }

            /** Case for not full inverted page table **/
            else
            {
              insertTable(pagetable_tree, inverted_pagetable, tmp->ppnnode);
              dq->amu += inverted_pagetable->count;
              enqueuePPNQueue(dq, tmp->ppnnode);
              list[pididx]->block = 0;
              dq->arp += 1;
              cursor = tmp->pos;
              break;
            }
          }

          /** Case for unblocked process **/
          else
          {
            // check page fault
            faultflag = checkPageFault(pagetable_tree, inverted_pagetable, pididx, vpn);
            
            /** Case for Hit **/
            if (faultflag > 0)
            {
              dq->amu += inverted_pagetable->count;
              dq->tmr++;
            }

            /** Case for Miss, No need Replacement **/
            else if (faultflag == 0)
            {
              /** Case for not full Disk **/
              if (dq->size > dq->count)
              {
                list[pididx]->block = 1;
                struct PPNNode *newNode = createPPNNode(inverted_pagetable, pid, vpn, pididx);
                enqueueDiskQueue(dq, newNode, cursor);
                dq->amu += inverted_pagetable->count;
                list[pididx++]->curpos = cursor;

                if (pididx == numprocess)
                {
                  pididx = pididx % numprocess;
                }
                cursor = list[pididx]->curpos;
                break;
              }

              /** Case for full Disk **/
              else
              {
                tmp = dequeueDiskQueue(dq, inverted_pagetable);
                list[tmp->ppnnode->index]->block = 0;
                dq->arp += 1;
                insertTable(pagetable_tree, inverted_pagetable, tmp->ppnnode);
                dq->amu += inverted_pagetable->count;
                enqueuePPNQueue(dq, tmp->ppnnode);
                list[pididx]->block = 1;
                struct PPNNode *newNode = createPPNNode(inverted_pagetable, pid, vpn, pididx);
                enqueueDiskQueue(dq, newNode, cursor);
                list[pididx++]->curpos = cursor;

                if (pididx == numprocess)
                {
                  pididx = pididx % numprocess;
                }
                cursor = list[pididx]->curpos;
                break;
              }
            }

            /** Case for Miss, Need Replacement **/
            else
            {
              /** Case for Not Full **/
              if (dq->size > dq->count)
              {
                list[pididx]->block = 1;
                struct PPNNode *newNode = createPPNNode(inverted_pagetable, pid, vpn, pididx);
                enqueueDiskQueue(dq, newNode, cursor);
                dq->amu += inverted_pagetable->count;
                list[pididx++]->curpos = cursor;

                if (pididx == numprocess)
                {
                  pididx = pididx % numprocess;
                }
                cursor = list[pididx]->curpos;
                break;
              }

              /** Case for Full Disk **/
              else
              {
                tmp = dequeueDiskQueue(dq, inverted_pagetable);
                list[tmp->ppnnode->index]->block = 0;
                dq->arp += 1;
                list[tmp->ppnnode->index]->curpos = tmp->pos;
                struct PPNNode *oldNode = dequeuePPNQueue(dq, inverted_pagetable);
                deleteTable(pagetable_tree, inverted_pagetable, oldNode);
                insertTable(pagetable_tree, inverted_pagetable, tmp->ppnnode);
                dq->amu += inverted_pagetable->count;
                enqueuePPNQueue(dq, tmp->ppnnode);
                list[pididx]->block = 1;
                struct PPNNode *newNode = createPPNNode(inverted_pagetable, pid, vpn, pididx);
                enqueueDiskQueue(dq, newNode, cursor);
                list[pididx++]->curpos = cursor;

                if (pididx == numprocess)
                {
                  pididx = pididx % numprocess;
                }
                cursor = list[pididx]->curpos;
                break;
              }
            }
          }
        }

        /** Case for Last Trace File for the Process **/
        if (cursor == list[pididx]->finishing_reference)
        {
          list[pididx]->curpos = cursor;
          tmp_invCount = inverted_pagetable->count;
          deletePidTable(pagetable_tree, inverted_pagetable, pididx);
          dq->amu -= tmp_invCount - inverted_pagetable->count;
          list[pididx++]->done = 1;

          if (pididx == numprocess)
          {
            pididx = pididx % numprocess;
          }

          if (dq->size != 1)
          {
            dq->size--;
          }
          cursor = list[pididx]->curpos;
          total_pid_finished++;
          break;
        }
        // record the starting position of next line
        cursor = ftell(fp);
      }
    }
  }
  
  /** Statistics **/
  dq->amu = dq->amu / inverted_pagetable->size;
  dq->arp = dq->arp / (double)dq->runtime;
  dq->amu = dq->amu / (double)dq->runtime;
  
  fclose(fp);
  return dq;
}

/*
 * Function: lru
 * ----------------------------
 *  
 *   Returns: DiskQueue *dq
 * 
 *   char *filename : a trace file to read
 *   int pagetable_size : size of pagetable
 *   pidList *pidlist: list of pids and their finishing reference
 *
 *   This function reads the trace file and executes lru page replacement
 *   policy and returns DiskQueue object which contains the statistic results
 */
DiskQueue *lru(char *filename, int pagetable_size, pidList *pidlist)
{
  FILE *fp;
  fp = fopen(filename, "r");
  if (fp == NULL)
  {
    fprintf(stderr, "Error: failed to open the file.\n");
    exit(EXIT_FAILURE);
  }

  /** INITIALIZATION **/
  DiskQueue *dq = createDiskQueue(pidlist->size);
  InvertTree *inverted_pagetable = createInvertTree(pagetable_size);
  // InvertTree *inverted_pagetable = createInvertTree(4);
  ProcessTree *pagetable_tree = createProcTree(pidlist->size);
  for (int i = 0; i < pidlist->size; i++)
  {
    createPageTables(pagetable_tree, i, pidlist->list[i]->pid);
  }

  /** LINE READING VARIABLE **/
  char *line = NULL;
  size_t line_len;
  long int pid = 0;
  long int vpn = 0;
  long int cursor = 0;

  /** OTHER VARIABLE**/
  int faultflag = 0;
  DiskQueueNode *tmp = NULL;
  parseNode *line_parse = NULL;
  pidNode **list = pidlist->list;
  int tmp_invCount = 0;

  /** CURRENTLY WORKING PID **/
  int pididx = 0;
  int total_pid_finished = 0;
  int numprocess = pidlist->size;

  // set starting position
  cursor = list[pididx]->start_reference;

  while (1)
  {
    // stop when all processes are done
    if (total_pid_finished == numprocess)
    {
      break;
    }
    fseek(fp, cursor, SEEK_SET);

    // skip the finished process
    if (list[pididx]->done == 1)
    {
      pididx++;
      if (pididx == numprocess)
      {
        pididx = pididx % numprocess;
      }
      cursor = list[pididx]->curpos;
    }
    else
    {
      // record the starting position of the line
      cursor = ftell(fp);
      while (getline(&line, &line_len, fp) != EOF)
      {
        line_parse = getPidandVpn(line);
        pid = line_parse->pid;
        vpn = line_parse->vpn;
        free(line_parse);

        // only deal with the currently working process
        if (pid == list[pididx]->pid)
        {
          dq->runtime++;
          dq->arp += dq->size - dq->count;

          /** Case for Blocked Process **/
          if (list[pididx]->block == 1)
          {
            tmp = dequeueDiskQueue(dq, inverted_pagetable);

            /** Case for Full Inverted Page Table **/
            if (inverted_pagetable->size == inverted_pagetable->count)
            {
              struct PPNNode *oldNode = dequeuePPNQueue(dq, inverted_pagetable);
              deleteTable(pagetable_tree, inverted_pagetable, oldNode);
              insertTable(pagetable_tree, inverted_pagetable, tmp->ppnnode);
              dq->amu += inverted_pagetable->count;
              enqueuePPNQueue(dq, tmp->ppnnode);
              list[pididx]->block = 0;
              dq->arp += 1;
              cursor = tmp->pos;
              break;
            }

            /** Case for Not Full Page Table **/
            else
            {
              insertTable(pagetable_tree, inverted_pagetable, tmp->ppnnode);
              dq->amu += inverted_pagetable->count;
              enqueuePPNQueue(dq, tmp->ppnnode);
              list[pididx]->block = 0;
              dq->arp += 1;
              cursor = tmp->pos;
              break;
            }
          }

          /** Case for Unblocked Process **/
          else
          {
            // check page fault
            faultflag = checkPageFault(pagetable_tree, inverted_pagetable, pididx, vpn);
            
            /** Case for Hit **/
            if (faultflag > 0)
            {
              dq->amu += inverted_pagetable->count;
              dq->tmr++;
              struct PPNNode *temp = getPPNNode(inverted_pagetable, faultflag);
              replacePPNQueue(dq, temp);
            }
            /** Case for Miss, No Need Replacement **/
            else if (faultflag == 0)
            {
              
              /** Case for Not Full Disk **/
              if (dq->size > dq->count)
              {
                list[pididx]->block = 1;
                struct PPNNode *newNode = createPPNNode(inverted_pagetable, pid, vpn, pididx);
                enqueueDiskQueue(dq, newNode, cursor);
                dq->amu += inverted_pagetable->count;
                list[pididx++]->curpos = cursor;
                
                if (pididx == numprocess)
                {
                  pididx = pididx % numprocess;
                }
                cursor = list[pididx]->curpos;
                break;
              }

              /** Case for Full Disk **/
              else
              {
                tmp = dequeueDiskQueue(dq, inverted_pagetable);
                list[tmp->ppnnode->index]->block = 0;
                dq->arp += 1;
                insertTable(pagetable_tree, inverted_pagetable, tmp->ppnnode);
                dq->amu += inverted_pagetable->count;
                enqueuePPNQueue(dq, tmp->ppnnode);
                list[pididx]->block = 1;
                struct PPNNode *newNode = createPPNNode(inverted_pagetable, pid, vpn, pididx);
                enqueueDiskQueue(dq, newNode, cursor);
                
                list[pididx++]->curpos = cursor;
                if (pididx == numprocess)
                {
                  pididx = pididx % numprocess;
                }
                cursor = list[pididx]->curpos;
                break;
              }
            }

            /** Case for Miss, Need Replacement **/
            else
            {
              /** Case for Not Full Disk **/
              if (dq->size > dq->count)
              {
                list[pididx]->block = 1;
                struct PPNNode *newNode = createPPNNode(inverted_pagetable, pid, vpn, pididx);
                enqueueDiskQueue(dq, newNode, cursor);
                dq->amu += inverted_pagetable->count;
                list[pididx++]->curpos = cursor;

                if (pididx == numprocess)
                {
                  pididx = pididx % numprocess;
                }
                cursor = list[pididx]->curpos;
                break;
              }
              
              /** Case for Full Disk **/
              else
              {
                tmp = dequeueDiskQueue(dq, inverted_pagetable);
                list[tmp->ppnnode->index]->block = 0;
                dq->arp += 1;
                list[tmp->ppnnode->index]->curpos = tmp->pos;
                struct PPNNode *oldNode = dequeuePPNQueue(dq, inverted_pagetable);
                deleteTable(pagetable_tree, inverted_pagetable, oldNode);
                insertTable(pagetable_tree, inverted_pagetable, tmp->ppnnode);
                dq->amu += inverted_pagetable->count;
                enqueuePPNQueue(dq, tmp->ppnnode);
                list[pididx]->block = 1;
                struct PPNNode *newNode = createPPNNode(inverted_pagetable, pid, vpn, pididx);
                enqueueDiskQueue(dq, newNode, cursor);
                list[pididx++]->curpos = cursor;
                
                if (pididx == numprocess)
                {
                  pididx = pididx % numprocess;
                }
                cursor = list[pididx]->curpos;
                break;
              }
            }
          }
        }

        /** Case for Last Trace File of the Process **/
        if (cursor == list[pididx]->finishing_reference)
        {
          list[pididx]->curpos = cursor;
          tmp_invCount = inverted_pagetable->count;
          deletePidTable(pagetable_tree, inverted_pagetable, pididx);
          dq->amu -= tmp_invCount - inverted_pagetable->count;
          list[pididx++]->done = 1;
          
          if (pididx == numprocess)
          {
            pididx = pididx % numprocess;
          }

          // resize the DiskQueue
          if (dq->size != 1)
          {
            dq->size--;
          }
          // relocate the file cursor to next working process
          cursor = list[pididx]->curpos;
          // increment the number of finished processes
          total_pid_finished++;
          break;
        }
        // record the starting position of next line
        cursor = ftell(fp);
      }
    }
  }
  
  /** Statistics **/
  dq->amu = dq->amu / inverted_pagetable->size;
  dq->arp = dq->arp / (double)dq->runtime;
  dq->amu = dq->amu / (double)dq->runtime;
  fclose(fp);
  return dq;
}

/*
 * Function: clock_replacement
 * ----------------------------
 *  
 *   Returns: DiskQueue *dq
 * 
 *   char *filename : a trace file to read
 *   int pagetable_size : size of pagetable
 *   pidList *pidlist: list of pids and their finishing reference
 *
 *   This function reads the trace file and executes clock page replacement
 *   policy and returns DiskQueue object which contains the statistic results
 */
DiskQueue *clock_replacement(char *filename, int pagetable_size, pidList *pidlist)
{
  FILE *fp;
  fp = fopen(filename, "r");
  if (fp == NULL)
  {
    fprintf(stderr, "Error: failed to open the file.\n");
    exit(EXIT_FAILURE);
  }

  /** INITIALIZATION **/
  DiskQueue *dq = createDiskQueue(pidlist->size);
  InvertTree *inverted_pagetable = createInvertTree(pagetable_size);
  ProcessTree *pagetable_tree = createProcTree(pidlist->size);
  for (int i = 0; i < pidlist->size; i++)
  {
    createPageTables(pagetable_tree, i, pidlist->list[i]->pid);
  }

  /** LINE READING VARIABLE **/
  char *line = NULL;
  size_t line_len;
  long int pid = 0;
  long int vpn = 0;
  long int cursor = 0;

  /** OTHER VARIABLE**/
  int faultflag = 0;
  DiskQueueNode *tmp = NULL;
  parseNode *line_parse = NULL;
  pidNode **list = pidlist->list;
  int tmp_invCount = 0;

  /** CURRENTLY WORKING PID **/
  int pididx = 0;
  int total_pid_finished = 0;
  int numprocess = pidlist->size;

  // set starting position
  cursor = list[pididx]->start_reference;

  while (1)
  {
    // stop when all processes are done
    if (total_pid_finished == numprocess)
    {
      break;
    }
    fseek(fp, cursor, SEEK_SET);

    // skip the finished process
    if (list[pididx]->done == 1)
    {
      pididx++;
      if (pididx == numprocess)
      {
        pididx = pididx % numprocess;
      }
      cursor = list[pididx]->curpos;
    }
    else
    {
      // record the starting position of the line
      cursor = ftell(fp);
      while (getline(&line, &line_len, fp) != EOF)
      {
        line_parse = getPidandVpn(line);
        pid = line_parse->pid;
        vpn = line_parse->vpn;
        free(line_parse);

        // only deal with the currently working process
        if (pid == list[pididx]->pid)
        {
          dq->runtime++;
          dq->arp += dq->size - dq->count;

          /** Case for Blocked Process **/
          if (list[pididx]->block == 1)
          {
            tmp = dequeueDiskQueue(dq, inverted_pagetable);
           
            /** Case for Full Inverted Page Table **/
            if (inverted_pagetable->size == inverted_pagetable->count)
            {
              struct PPNNode *oldNode = dequeueClockQueue(dq);
              if (oldNode == NULL)
              {
                fprintf(stderr, "Block inverted page is full\n");
                exit(EXIT_FAILURE);
              }

              deleteTable(pagetable_tree, inverted_pagetable, oldNode);
              insertTable(pagetable_tree, inverted_pagetable, tmp->ppnnode);
              dq->amu += inverted_pagetable->count;
              enqueueClockQueue(dq, tmp->ppnnode);
              list[pididx]->block = 0;
              dq->arp += 1;
              cursor = tmp->pos;
              break;
            }

            /** Case for Not Full Inverted Page Table **/
            else
            {
              insertTable(pagetable_tree, inverted_pagetable, tmp->ppnnode);
              dq->amu += inverted_pagetable->count;
              enqueueClockQueue(dq, tmp->ppnnode);
              list[pididx]->block = 0;
              dq->arp += 1;
              cursor = tmp->pos;
              break;
            }
          }
          
          /** Case for Unblocked Process **/
          else
          {
            // check page fault
            faultflag = checkPageFault(pagetable_tree, inverted_pagetable, pididx, vpn);

            /** Case for Hit **/
            if (faultflag > 0)
            {
              dq->amu += inverted_pagetable->count;
              dq->tmr++;
              struct PPNNode *temp = getPPNNode(inverted_pagetable, faultflag);
              temp->check = 1;
            }

            /** Case for Miss, No Need Replacement**/
            else if (faultflag == 0)
            {
              /** Case for Not Full Disk **/
              if (dq->size > dq->count)
              {
                list[pididx]->block = 1;
                struct PPNNode *newNode = createPPNNode(inverted_pagetable, pid, vpn, pididx);
                enqueueDiskQueue(dq, newNode, cursor);
                dq->amu += inverted_pagetable->count;
                
                list[pididx++]->curpos = cursor;
                if (pididx == numprocess)
                {
                  pididx = pididx % numprocess;
                }
                cursor = list[pididx]->curpos;
                break;
              }

              /** Case for Full Disk **/
              else
              {
                tmp = dequeueDiskQueue(dq, inverted_pagetable);
                list[tmp->ppnnode->index]->block = 0;
                dq->arp += 1;
                insertTable(pagetable_tree, inverted_pagetable, tmp->ppnnode);
                dq->amu += inverted_pagetable->count;
                enqueueClockQueue(dq, tmp->ppnnode);
                list[pididx]->block = 1;
                struct PPNNode *newNode = createPPNNode(inverted_pagetable, pid, vpn, pididx);
                enqueueDiskQueue(dq, newNode, cursor);
                list[pididx++]->curpos = cursor;

                if (pididx == numprocess)
                {
                  pididx = pididx % numprocess;
                }
                cursor = list[pididx]->curpos;
                break;
              }
            }

            /** Case for Miss, Need Replacement **/
            else
            {
              /** Case for Not Full Disk **/
              if (dq->size > dq->count)
              {
                list[pididx]->block = 1;
                struct PPNNode *newNode = createPPNNode(inverted_pagetable, pid, vpn, pididx);
                enqueueDiskQueue(dq, newNode, cursor);
                dq->amu += inverted_pagetable->count;
                list[pididx++]->curpos = cursor;

                if (pididx == numprocess)
                {
                  pididx = pididx % numprocess;
                }
                cursor = list[pididx]->curpos;
                break;
              }
              
              /** Case for Full Disk */
              else
              {
                tmp = dequeueDiskQueue(dq, inverted_pagetable);
                list[tmp->ppnnode->index]->block = 0;
                dq->arp += 1;
                list[tmp->ppnnode->index]->curpos = tmp->pos;
                struct PPNNode *oldNode = dequeueClockQueue(dq);
                deleteTable(pagetable_tree, inverted_pagetable, oldNode);
                insertTable(pagetable_tree, inverted_pagetable, tmp->ppnnode);
                dq->amu += inverted_pagetable->count;
                enqueueClockQueue(dq, tmp->ppnnode);
                list[pididx]->block = 1;
                struct PPNNode *newNode = createPPNNode(inverted_pagetable, pid, vpn, pididx);
                enqueueDiskQueue(dq, newNode, cursor);
                list[pididx++]->curpos = cursor;

                if (pididx == numprocess)
                {
                  pididx = pididx % numprocess;
                }
                cursor = list[pididx]->curpos;
                break;
              }
            }
          }
        }

        /** Case for Last Trace File for the Process **/
        if (cursor == list[pididx]->finishing_reference)
        {
          list[pididx]->curpos = cursor;
          tmp_invCount = inverted_pagetable->count;
          deletePidTable(pagetable_tree, inverted_pagetable, pididx);
          dq->amu -= tmp_invCount - inverted_pagetable->count;
          list[pididx++]->done = 1;
          
          if (pididx == numprocess)
          {
            pididx = pididx % numprocess;
          }
          
          // resize the DiskQueue
          if (dq->size != 1)
          {
            dq->size--;
          }
          // relocate the file cursor to next working process
          cursor = list[pididx]->curpos;
          // increment the number of finished processes
          total_pid_finished++;
          break;
        }
        // record the starting position of next line
        cursor = ftell(fp);
      }
    }
  }
  
  /** Statistics **/
  dq->amu = dq->amu / inverted_pagetable->size;
  dq->arp = dq->arp / (double)dq->runtime;
  dq->amu = dq->amu / (double)dq->runtime;
  fclose(fp);
  return dq;
}
