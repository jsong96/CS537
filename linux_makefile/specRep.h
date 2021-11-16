/***********************************************************************
* FILENAME :       specRep.h             
*
* DESCRIPTION :
*       This is the header file the nodes in the graph
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon  
* CLASS: CS 537
* TERM: FALL 2020
*
**/

#ifndef SPEC_REP
#define SPEC_REP

typedef struct node
{
  char *target;
  char **dependencies;
  char **command;
  int numDependencies;
  int numCommands;
} Node;

typedef struct graphnode
{
  Node *node;
  Node **children; // List of children
  int numChildren;
  int sizeChildren;
} GraphNode;


Node *createNodeSingle(char *tgt, char *dps, char **cmd, int cmdSize);

GraphNode *createNode(char *tgt, char *dps, char **cmd, int cmdSize);

#endif
