/***********************************************************************
* FILENAME :       pagetable.c             
*
* DESCRIPTION :
*       This is a source file for pagetable header file
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon
* CLASS: CS 537
* TERM: FALL 2020
*
**/
#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <search.h>
#include "pagetable.h"
#include "diskQueue.h"

/*
 * Function: createInvertTree
 * ----------------------------
 *  
 *   Returns: InvertTree *tree
 * 
 *   int size : The size of Inverted page table
 *
 *   This function initializes an inverted page table
 */
InvertTree *createInvertTree(int size)
{
  InvertTree *tree = malloc(sizeof(InvertTree));
  if (tree == NULL)
  {
    fprintf(stderr, "Error createInvertTree: malloc BTree");
    exit(EXIT_FAILURE);
  }
  tree->root = NULL;
  tree->count = 0;
  tree->size = size;
  tree->ppn = 1;

  return tree;
}

/*
 * Function: createProcTree
 * ----------------------------
 *  
 *   Returns: ProcessTree *tree
 * 
 *   int numpids :  The number of unique PID
 *
 *   This function initializes a storage for page tables of pids.
 *   Each index will store a page table for a PID
 */
ProcessTree *createProcTree(int numpids)
{
  ProcessTree *tree = malloc(sizeof(ProcessTree));
  if (tree == NULL)
  {
    fprintf(stderr, "Error createProcTree: malloc tree\n");
    exit(EXIT_FAILURE);
  }
  tree->numPid = numpids;
  tree->pids = malloc(sizeof(ProcessNode *) * numpids);
  if (tree->pids == NULL)
  {
    fprintf(stderr, "Error createProcTree: malloc ProcessNodes");
    exit(EXIT_FAILURE);
  }
  return tree;
}

/*
 * Function: createPageTables
 * ----------------------------
 *  
 *   ProcessTree *tree : Storage of page tables
 *   int index : Index of ProcessTree where a page table will be initialized
 *   unsigned long int pid : Process ID
 * 
 *   This function initializes a page table of a PID   
 */
void createPageTables(ProcessTree *tree, int index, unsigned long int pid)
{
  ProcessNode *node = malloc(sizeof(ProcessNode));
  if (node == NULL)
  {
    fprintf(stderr, "Error createPageTables: malloc ProcessNode\n");
    exit(EXIT_FAILURE);
  }
  node->root = NULL;
  node->dequeueCount = 0;
  node->linkedlistroot = NULL;
  node->linkedlisttail = NULL;
  node->pid = pid;

  tree->pids[index] = node;
}

/*
 * Function: comparePpn
 * ----------------------------
 *  
 *   Returns: node1->ppn - node2->ppn
 * 
 *   const void *n1 : PPNNode node
 *   const void *n2 : PPNNode node
 *   
 *   This function calculates the difference in
 *   the value of PPN between two different PPNNodes
 */
int comparePpn(const void *n1, const void *n2)
{
  struct PPNNode *node1 = (struct PPNNode *)n1;
  struct PPNNode *node2 = (struct PPNNode *)n2;
  return node1->ppn - node2->ppn;
}

/*
 * Function: compareVpn
 * ----------------------------
 *  
 *   Returns: node1->vpn - node2->vpn
 * 
 *   const void *n1 : VPNNode
 *   const void *n1 : VPNNode
 * 
 *   This function calculates the difference in
 *   the value of VPN between two different VPNNodes
 */
int compareVpn(const void *n1, const void *n2)
{
  struct VPNNode *node1 = (struct VPNNode *)n1;
  struct VPNNode *node2 = (struct VPNNode *)n2;
  return node1->vpn - node2->vpn;
}

/*
 * Function: createPPNNode
 * ----------------------------
 *  
 *   Returns: struct PPNNode *newNode
 * 
 *   InvertTree *inverttree : Inverted Page Table
 *   long int pid : PID for newNode
 *   long int vpn : VPN for newNode
 *   int index : Index for PID in ProcessTree
 *  
 *   This function initializes a new PPNNode with given information
 *   and assigns a unique PPN for the node.   
 */
struct PPNNode *createPPNNode(InvertTree *inverttree, long int pid, long int vpn, int index)
{
  struct PPNNode *newNode = malloc(sizeof(struct PPNNode));
  if (newNode == NULL)
  {
    fprintf(stderr, "Error createNode: malloc newNode\n");
    exit(EXIT_FAILURE);
  }
  newNode->pid = pid;
  newNode->vpn = vpn;
  newNode->ppn = inverttree->ppn;
  newNode->index = index;
  newNode->check = 0;
  newNode->check1 = 1;

  inverttree->ppn++;
  newNode->next = NULL;
  newNode->prev = NULL;
  struct VPNNode *vpnnode = malloc(sizeof(struct VPNNode));
  if (vpnnode == NULL)
  {
    fprintf(stderr, "Error createPPNNode: malloc vpnnode");
    exit(EXIT_FAILURE);
  }

  vpnnode->pid = pid;
  vpnnode->vpn = vpn;
  vpnnode->next = NULL;
  vpnnode->prev = NULL;
  vpnnode->ppnnode = newNode;
  newNode->vpnnode = vpnnode;
  return newNode;
}

/*
 * Function: checkPageFault
 * ----------------------------
 *  
 *   Returns: long int PPN or 0 or -1
 * 
 *   ProcessTree *proctree : Storage for page tables 
 *   InvertTree *inverttree : Inverted page table
 *   int index : Index for PID in ProcessTree
 *   long int vpn : VPN for PID to be searched
 * 
 *   This function returns PPN value from ProcessTree. If not found,
 *   returns -1 for pagefault that needs replacement or 0 for miss   
 */
long int checkPageFault(ProcessTree *proctree, InvertTree *inverttree, int index, long int vpn)
{
  void *result = NULL;
  ProcessNode *procnode = proctree->pids[index];
  struct VPNNode *vpnnode = malloc(sizeof(struct VPNNode));
  if (vpnnode == NULL)
  {
    fprintf(stderr, "Error checkPageFault: malloc vpnnode");
    exit(EXIT_FAILURE);
  }
  vpnnode->vpn = vpn;
  
  result = tfind(vpnnode, &procnode->root, compareVpn);

  if (result == NULL)
  {
    if (inverttree->size > inverttree->count)
    {
      return 0;
    }
    return -1;
  }

  struct VPNNode *temp = *(struct VPNNode **)result;

  long int ppn = temp->ppnnode->ppn;
  return ppn;
}

/*
 * Function: deleteTable
 * ----------------------------
 *  
 *   ProcessTree *proctree : Storage for page tables
 *   InvertTree *inverttree : Inverted page table
 *   struct PPNNode *ppnnode : PPNNode to be deleted 
 *
 *   This function deletes PPNNode from ProcessTree and InvertTree
 */
void deleteTable(ProcessTree *proctree, InvertTree *inverttree, struct PPNNode *ppnnode)
{
  void *result = NULL;
  
  // Delete from page table
  ProcessNode *procnode = proctree->pids[ppnnode->index];

  result = tdelete(ppnnode->vpnnode, &procnode->root, compareVpn);
  if (result == NULL)
  {
    printf("PID: %ld, VPN %ld PPN %ld\n", ppnnode->pid, ppnnode->vpn, ppnnode->ppn);
    fprintf(stderr, "Error deleteTable: tdelete vpnnode\n");
    exit(EXIT_FAILURE);
  }

  // Delete from Inverted table
  result = tdelete(ppnnode, &inverttree->root, comparePpn);
  if (result == NULL)
  {
    printf("PID: %ld, VPN %ld PPN %ld\n", ppnnode->pid, ppnnode->vpn, ppnnode->ppn);
    fprintf(stderr, "Error deleteTable: tdelete node\n");
    exit(EXIT_FAILURE);
  }
  inverttree->count--;

  // Update Linked List of VPNNode
  if (ppnnode->vpnnode->prev == NULL && ppnnode->vpnnode->next == NULL)
  {
    procnode->linkedlistroot = NULL;
    procnode->linkedlisttail = NULL;
  }
  else if (ppnnode->vpnnode->prev == NULL)
  {
    procnode->linkedlistroot = ppnnode->vpnnode->next;
    ppnnode->vpnnode->next->prev = NULL;
  }
  else if (ppnnode->vpnnode->next == NULL)
  {
    ppnnode->vpnnode->prev->next = NULL;
    procnode->linkedlisttail = ppnnode->vpnnode->prev;
  }
  else
  {
    ppnnode->vpnnode->prev->next = ppnnode->vpnnode->next;
    ppnnode->vpnnode->next->prev = ppnnode->vpnnode->prev;
  }
}

/*
 * Function: insertTable
 * ----------------------------
 *   
 *   ProcessTree *proctree : Storage for page tables
 *   InvertTree *invertree : Inverted page table
 *   struct PPNNode *ppnode : PPNNode to be inserted
 *
 *   This function inserts PPNNode into ProcessTree and InvertTree
 */
void insertTable(ProcessTree *proctree, InvertTree *inverttree, struct PPNNode *ppnnode)
{
  void *result = NULL;

  // Insert node into Invert Tree
  result = tsearch(ppnnode, &inverttree->root, comparePpn);
  if (result == NULL)
  {
    fprintf(stderr, "Error insertTable: tsearch newNode invertTree\n");
    exit(EXIT_FAILURE);
  }
  inverttree->count++;

  // Insert node into Page Table
  ProcessNode *procnode = proctree->pids[ppnnode->index];
  result = tsearch(ppnnode->vpnnode, &procnode->root, compareVpn);
  if (result == NULL)
  {
    fprintf(stderr, "Error insertTable: tsearch newNode pidTree\n");
    exit(EXIT_FAILURE);
  }

  // Update linked list of VPNNode
  if (procnode->linkedlistroot == NULL)
  {
    procnode->linkedlistroot = ppnnode->vpnnode;
    procnode->linkedlisttail = ppnnode->vpnnode;
  }
  else
  {
    procnode->linkedlisttail->next = ppnnode->vpnnode;
    ppnnode->vpnnode->prev = procnode->linkedlisttail;
    procnode->linkedlisttail = ppnnode->vpnnode;
  }
}

/*
 * Function: deletePidTable
 * ----------------------------
 *   
 *   ProcessTree *proctree : Storage for page tables
 *   InvertTree *inverttree : Inverted page table
 *   int index : index for PID in ProcessTree
 *   
 *   This function deletes all PPNs that are related to the PID from Inverted page table
 */
void deletePidTable(ProcessTree *proctree, InvertTree *inverttree, int index)
{

  void *result = NULL;

  ProcessNode *procnode = proctree->pids[index];
  long int pid = procnode->pid;
  struct VPNNode *tmp = procnode->linkedlistroot;
  while (tmp != NULL)
  {
    result = tdelete(tmp->ppnnode, &inverttree->root, comparePpn);
    if (result != NULL)
    {
      procnode->dequeueCount++;
    }
    
    
    tmp->ppnnode->ppn = 0;
    tmp = tmp->next;
  }
  inverttree->count -= procnode->dequeueCount;
  free(procnode);

  printf("DELETING TABLE PID: %ld and INVERTEDTREE COUNT: %d\n", pid, inverttree->count);
}

/*
 * Function: getPPNNode
 * ----------------------------
 *  
 *   Returns: struct PPNNode *ppnnode
 * 
 *   InvertTree *inverttree : Inverted page table
 *   long int ppn : PPN to be searched
 *
 *   This function returns PPNNode from InvertTree, that matches
 *   with the PPN
 */
struct PPNNode *getPPNNode(InvertTree *inverttree, long int ppn)
{
  void *result = NULL;
  struct PPNNode *ppnnode = malloc(sizeof(struct PPNNode));
  if (ppnnode == NULL)
  {
    fprintf(stderr, "Error getPPNNode: malloc ppnnode");
    exit(EXIT_FAILURE);
  }
  ppnnode->ppn = ppn;

  result = tsearch(ppnnode, &inverttree->root, comparePpn);
  free(ppnnode);

  ppnnode = *(struct PPNNode **)result;
  return ppnnode;
}
