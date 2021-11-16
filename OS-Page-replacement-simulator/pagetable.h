/***********************************************************************
* FILENAME :       pagetable.h             
*
* DESCRIPTION :
*       This is the header file for pagetable.c
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon
* CLASS: CS 537
* TERM: FALL 2020
*
**/
#ifndef PAGE_TABLE
#define PAGE_TABLE

struct PPNNode
{
  long int pid;
  long int vpn;
  long int ppn;
  int index;
  int check;
  int check1;
  struct PPNNode *next;
  struct PPNNode *prev;
  struct VPNNode *vpnnode;
};

struct VPNNode
{
  long int pid;
  long int vpn;
  struct VPNNode *next;
  struct VPNNode *prev;
  struct PPNNode *ppnnode;
};

typedef struct inverttree
{
  void *root;
  int count;
  int size;
  long int ppn;
} InvertTree;


typedef struct processnode
{
  unsigned long int pid;
  struct VPNNode *linkedlistroot;
  struct VPNNode *linkedlisttail;
  void *root;
  int dequeueCount;
} ProcessNode;

typedef struct processtree
{
  ProcessNode **pids;
  int numPid;
} ProcessTree;

InvertTree *createInvertTree(int size);

ProcessTree *createProcTree(int numpids);

int comparePid(const void *n1, const void *n2);

void createPageTables(ProcessTree *tree, int index, unsigned long int pid);

int comparePpn(const void *n1, const void *n2);

struct PPNNode *createPPNNode(InvertTree *inverttree, long int pid, long int vpn, int index);

int compareVpn(const void *n1, const void *n2);

long int checkPageFault(ProcessTree *proctree, InvertTree *inverttree, int index, long int vpn);

void deleteTable(ProcessTree *proctree, InvertTree *inverttree, struct PPNNode *ppnnode);

void insertTable(ProcessTree *proctree, InvertTree *inverttree, struct PPNNode *ppnnode);

void deletePidTable(ProcessTree *proctree, InvertTree *inverttree, int index);

struct PPNNode *getPPNNode(InvertTree *inverttree, long int ppn);

#endif
