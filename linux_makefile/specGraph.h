/***********************************************************************
* FILENAME :       specGraph.h             
*
* DESCRIPTION :
*       This is the header file for graph structure
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon 
* CLASS: CS 537
* TERM: FALL 2020
*
**/

#ifndef SPEC_GRAPH
#define SPEC_GRAPH

#include "specRep.h"
#include "createExecute.h"

typedef struct graph
{
  //Node **nodes;
  GraphNode **nodes;
  int size;
  int numNode;

} Graph;

typedef struct visitnode
{
  GraphNode **visited;
  int numVisit;
} VisitNode;

typedef struct topnode
{
  GraphNode **topNodes;
  int numTop;

} TopNode;

typedef struct command
{
  char *target;
  char **cmd;
  int cmdSize;
} Command;

typedef struct commands
{
  Command **cmds;
  int cmdsSize;
  int cmdsIndex;
} Commands;

Graph *createGraph();

void addNode(Graph *graph, GraphNode *newNode);

int recursive(Graph *graph, GraphNode *currNode, VisitNode *visit);

int detectCycle(Graph *graph);

void addConnection(Graph *graph);

Commands *traverse(Graph *graph, char *targetObject);

int traverseRecursion(Graph *graph, VisitNode *visit, GraphNode *currNode, Commands *cmds);

TopNode *topNodes(Graph *graph);

time_t getModificationTime(char *filePath);

#endif