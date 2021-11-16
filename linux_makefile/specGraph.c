/***********************************************************************
* FILENAME :       specGraph.c             
*
* DESCRIPTION : 
*       This is source file for specGraph header file.
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon
* CLASS: CS 537
* TERM: FALL 2020
*
**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "specRep.h"
#include "specGraph.h"

/*
 * Function: createGraph
 * ----------------------------
 *  Return: Graph *g
 *
 *  This function initializes a new directed Graph and returns the graph.
 */
Graph *createGraph()
{
  Graph *g = (Graph *)malloc(sizeof(Graph));
  if (g == NULL)
  {
    fprintf(stderr, "Error: Failed to allocate memory for malloc(Graph).\n");
    exit(EXIT_FAILURE);
  }
  g->nodes = malloc(2 * sizeof(GraphNode *));
  if (g->nodes == NULL)
  {
    fprintf(stderr, "Error: failed to allocate memory for graph node.\n");
    exit(EXIT_FAILURE);
  }
  g->size = 2;
  g->numNode = 0;
  return g;
}

/*
 * Function: addNode
 * ----------------------------
 *
 *   graph: size of new Queue
 *   newNode: New GraphNode to be inserted into the graph
 *
 *   This function inserts new node into the graph
 */
void addNode(Graph *graph, GraphNode *newNode)
{

  if (graph == NULL)
  {
    fprintf(stderr, "Error: graph is NULL\n");
    exit(EXIT_FAILURE);
  }
  if (newNode == NULL)
  {
    fprintf(stderr, "Error: New node is NULL\n");
    exit(EXIT_FAILURE);
  }

  GraphNode *currNode = graph->nodes[0];

  /* First insertion of node */
  if (currNode == NULL)
  {
    graph->nodes[0] = newNode;
    graph->numNode++;
    return;
  }

  GraphNode *tempNode = NULL;

  /* Check for duplication before inserting new node */
  for (int i = 0; i < graph->numNode; i++)
  {
    tempNode = graph->nodes[i];

    if (strcmp(tempNode->node->target, newNode->node->target) == 0)
    {
      return;
    }
  }

  /* Expand graph when it is full of nodes */
  if (graph->numNode >= graph->size)
  {
    int newSize = graph->size * 2;
    graph->nodes = realloc(graph->nodes, newSize * sizeof(GraphNode));
    if (graph->nodes == NULL)
    {
      fprintf(stderr, "Error: Failed to allocate memory for graph->nodes\n");
      exit(EXIT_FAILURE);
    }
    graph->size = newSize;
  }

  graph->nodes[graph->numNode] = newNode;
  graph->numNode++;
}

/*
 * Function: addConnection
 * ----------------------------
 *
 *   graph: Graph that requires nodes to be connected each other
 *
 *   This function connects GraphNodes of the graph according to dependencies
 */
void addConnection(Graph *graph)
{
  GraphNode *currNode = NULL;
  GraphNode *parentNode = NULL;

  for (int i = 0; i < graph->numNode; i++)
  {
    currNode = graph->nodes[i];

    for (int j = 0; j < graph->numNode; j++)
    {
      parentNode = graph->nodes[j];
      for (int k = 0; k < parentNode->node->numDependencies; k++)
      {
        if (strcmp(parentNode->node->dependencies[k], currNode->node->target) == 0)
        {
          if (parentNode->children == NULL)
          {
            parentNode->children = malloc(2 * sizeof(Node));
            if (parentNode->children == NULL) {
              fprintf(stderr, "Error: failed to allocate memory.\n");
              exit(EXIT_FAILURE);
            }
            parentNode->sizeChildren = 2;
          }
          else if (parentNode->numChildren >= parentNode->sizeChildren)
          {
            int newSize = parentNode->numChildren * 2;
            parentNode->children = realloc(parentNode->children, newSize * sizeof(Node));
            if (parentNode->children == NULL) {
              fprintf(stderr, "Error: failed to allocate memory.\n");
              exit(EXIT_FAILURE);
            }
          }
          parentNode->children[parentNode->numChildren] = currNode->node;
          parentNode->numChildren++;
        }
      }
    }
  }
}

/*
 * Function: recursive
 * ----------------------------
 *   Return: 1 if cycle is detected. Otherwise, 0
 *
 *   graph: Graph used in this program
 *   currNode: current GraphNode in the graph
 *   visit: List of GraphNodes that are already visited
 *
 *   This function is recursion to detect cycle in this graph.
 */
int recursive(Graph *graph, GraphNode *currNode, VisitNode *visit)
{
  // No child is found in the current node
  if (currNode->children == NULL)
  {
    return 0;
  }

  GraphNode *childGraphNode = NULL;
  Node *childNode = NULL;

  for (int i = 0; i < currNode->numChildren; i++)
  {
    childNode = currNode->children[i];

    /* Check cycle by number of visits */
    if (visit->numVisit >= graph->numNode)
    {
      return 1;
    }

    /* Find GraphNode of child node */
    for (int k = 0; k < graph->numNode; k++)
    {
      if (strcmp(graph->nodes[k]->node->target, childNode->target) == 0)
      {
        childGraphNode = graph->nodes[k];
      }
    }

    /* Check if the child node is visited*/
    for (int j = 0; j < visit->numVisit; j++)
    {
      if (strcmp(childNode->target, visit->visited[j]->node->target) == 0)
      {
        return 1;
      }
    }

    /* Recursion if the child node is not visited */
    visit->visited[visit->numVisit] = childGraphNode;
    visit->numVisit++;

    if (recursive(graph, childGraphNode, visit) == 1)
    {
      return 1;
    }

    visit->numVisit--;
  }
  return 0;
}

/*
 * Function: detectCycle
 * ----------------------------
 *   Return: 1 if a cycle in the graph is detected. Otherwise, 0/
 *
 *   graph: Graph to be checked if there is a cycle
 *
 *   This function calls recursive function for recursion to detect cycle.
 */
int detectCycle(Graph *graph)
{
  VisitNode *visit = (VisitNode *)malloc(sizeof(VisitNode));
  if (visit == NULL)
  {
    fprintf(stderr, "Error: Failed to allocate memory for Visit\n");
    exit(EXIT_FAILURE);
  }
  visit->visited = malloc(graph->numNode * sizeof(GraphNode));
  if (visit->visited == NULL)
  {
    fprintf(stderr, "Error: Failed to allocate memory for visited Nodes\n");
    exit(EXIT_FAILURE);
  }
  visit->numVisit = 0;

  /* Check cycle for every node of graph */
  for (int i = 0; i < graph->numNode; i++)
  {
    visit->visited[i] = graph->nodes[i];
    visit->numVisit = 1;

    if (recursive(graph, graph->nodes[i], visit) == 1)
    {
      free(visit->visited);
      free(visit);
      return 1;
    }
  }
  return 0;
}

/*
 * Function: topNodes
 * ----------------------------
 *   Return: TopNode *top
 *
 *   graph: Graph of this makefile
 *
 *   This function gets the list of roots of the graph.
 */
TopNode *topNodes(Graph *graph)
{
  TopNode *top = malloc(sizeof(TopNode));
  if (top == NULL)
  {
    fprintf(stderr, "Error: Failed to allocate memory for TopNode\n");
    exit(EXIT_FAILURE);
  }
  top->topNodes = malloc(graph->numNode * sizeof(GraphNode));
  if (top->topNodes == NULL)
  {
    fprintf(stderr, "Error: Failed to allocate memory for Top nodes\n");
    exit(EXIT_FAILURE);
  }
  top->numTop = 0;
  int check = 1;

  GraphNode *currNode = NULL;
  char *topTarget = NULL;

  /* Get roots of this graph */
  for (int i = 0; i < graph->numNode; i++)
  {
    check = 1;
    topTarget = graph->nodes[i]->node->target;

    for (int j = 0; j < graph->numNode; j++)
    {
      currNode = graph->nodes[j];

      for (int k = 0; k < currNode->node->numDependencies; k++)
      {
        if (strcmp(currNode->node->dependencies[k], topTarget) == 0)
        {
          check = 0;
        }
      }
    }

    if (check == 1)
    {
      /* Exclude clean */
      if (strcmp(graph->nodes[i]->node->target, "clean") != 0)
      {
        top->topNodes[top->numTop] = graph->nodes[i];
        top->numTop++;
      }
    }
  }
  return top;
}

/*
 * Function: getModificationTime
 * ----------------------------
 *   Return: time_t tempStat.st_mtime
 * 
 *   filePath: A string for the path of a file
 *
 *   This function returns the last modification of time of the file
 */
time_t getModificationTime(char *filePath)
{
  struct stat tempStat;
  if (stat(filePath, &tempStat) == 0)
  {
    return tempStat.st_mtime;
  }
  return 0;
}

/*
 * Function: traverseRecursion
 * ----------------------------
 *   Return: 1 if target is updated. Otherwise, 0
 *
 *   graph: Graph used in this program
 *   visit: List of GraphNodes that are visited
 *   currNode: Current GraphNode
 *   commands: List of Command to be executed
 *
 *   This function is recursion and inserts Command into Commands in post-order traversal
 */
int traverseRecursion(Graph *graph, VisitNode *visit, GraphNode *currNode, Commands *commands)
{
  time_t currentTime = 0;
  time_t dependencyTime = 0;
  int needUpdate = 0;
  double diff = 0;

  /* Leaf node */
  if (currNode->numChildren == 0)
  {
    /* Check modification time for dependencies */
    currentTime = getModificationTime(currNode->node->target);

    needUpdate = 0;

    /* Compare modification time between dependencies and target */
    for (int i = 0; i < currNode->node->numDependencies; i++)
    {
      dependencyTime = getModificationTime(currNode->node->dependencies[i]);
      diff = difftime(currentTime, dependencyTime);

      if (diff <= 0)
      {
        needUpdate = 1;
      }
    }
    /* Case if there is no dependency for the target */
    if (difftime(currentTime, dependencyTime) == 0)
    {
      needUpdate = 1;
    }

    /* Current target is already updated */
    if (needUpdate == 0)
    {
      return 0;
    }

    /* Realloc Commands if there is no available space */
    if (commands->cmdsIndex >= commands->cmdsSize)
    {
      commands->cmdsSize = commands->cmdsSize * 2;
      commands->cmds = (Command **)realloc(commands->cmds, commands->cmdsSize * sizeof(Command *));
      if (commands->cmds == NULL)
      {
        fprintf(stderr, "Error: Failed to allocate memory for commands->cmds\n");
        exit(EXIT_FAILURE);
      }
    }

    /* Insert Command of current Node */
    Command *command = malloc(sizeof(Command));
    if (command == NULL)
    {
      fprintf(stderr, "Error: failed to allocate memory for command struct.\n");
      exit(EXIT_FAILURE);
    }
    command->target = currNode->node->target;
    command->cmdSize = currNode->node->numCommands;
    command->cmd = currNode->node->command;
    commands->cmds[commands->cmdsIndex++] = command;

    /* Mark Current Node visited */
    visit->visited[visit->numVisit] = currNode;
    visit->numVisit++;
    return 1;
  }

  /* Recursion if there is a child node */
  GraphNode *childGraphNode = NULL;
  for (int i = 0; i < currNode->numChildren; i++)
  {
    for (int j = 0; j < graph->numNode; j++)
    {
      if (strcmp(graph->nodes[j]->node->target, currNode->children[i]->target) == 0)
      {
        childGraphNode = graph->nodes[j];
        int temp = traverseRecursion(graph, visit, childGraphNode, commands);
        if (temp == 1)
        {
          needUpdate = 1;
        }
      }
    }
  }

  /* Execute current node*/
  if (needUpdate == 1)
  {
    if (commands->cmdsIndex >= commands->cmdsSize)
    {
      commands->cmdsSize = commands->cmdsSize * 2;
      commands->cmds = (Command **)realloc(commands->cmds, commands->cmdsSize * sizeof(Command *));
      if (commands->cmds == NULL)
      {
        fprintf(stderr, "Error: Failed to allocate memory for commands->cmds \n");
        exit(EXIT_FAILURE);
      }
    }

    Command *command = malloc(sizeof(Command));
    if (command == NULL)
    {
      fprintf(stderr, "Error: failed to allocate memory for command struct.\n");
      exit(EXIT_FAILURE);
    }
    command->target = currNode->node->target;
    command->cmdSize = currNode->node->numCommands;
    command->cmd = currNode->node->command;
    commands->cmds[commands->cmdsIndex++] = command;
    return 1;
  }

  /* Mark Current Node visited */
  visit->visited[visit->numVisit] = currNode;
  visit->numVisit++;
  return 0;
}

/*
 * Function: traverse
 * ----------------------------
 *   Return: Commands commands
 *
 *   graph: Graph of this program
 *   targetObject: String for target dependency to be executed
 *
 *   This function calls traverseRecursion to execute command for target dependency
 */
Commands *traverse(Graph *graph, char *targetObject)
{
  /* Initializes Commands */
  Commands *commands = (Commands *)malloc(sizeof(Commands));
  if (commands == NULL)
  {
    fprintf(stderr, "Error: failed to allocate memory for commands struct.\n");
    exit(EXIT_FAILURE);
  }
  commands->cmdsSize = 1;
  commands->cmdsIndex = 0;
  commands->cmds = (Command **)malloc(sizeof(Command) * commands->cmdsSize);
  if (commands->cmds == NULL)
  {
    fprintf(stderr, "Error: failed to allocate memory for commands struct.\n");
    exit(EXIT_FAILURE);
  }

  /* Initialize VisitNode */
  VisitNode *visit = malloc(sizeof(VisitNode));
  if (visit == NULL)
  {
    fprintf(stderr, "Error: Failed to allocate memory for Visit\n");
    exit(EXIT_FAILURE);
  }
  visit->visited = malloc(graph->numNode * sizeof(GraphNode));
  if (visit->visited == NULL)
  {
    fprintf(stderr, "Error: Failed to allocate memory for visited Nodes\n");
    exit(EXIT_FAILURE);
  }
  visit->numVisit = 0;

  /* Execute target dependency*/
  if (targetObject == NULL)
  {
    TopNode *top = topNodes(graph);

    for (int i = 0; i < top->numTop; i++)
    {
      traverseRecursion(graph, visit, top->topNodes[i], commands);
    }
  }
  else
  {
    int check = 0;
    for (int i = 0; i < graph->numNode; i++)
    {
      if (strcmp(graph->nodes[i]->node->target, targetObject) == 0)
      {
        check = 1;
        traverseRecursion(graph, visit, graph->nodes[i], commands);
        return commands;
      }
    }

    /* Target dependency is not found */
    if (check == 0)
    {
      fprintf(stderr, "Error: There is no such target in the makefile\n");
      exit(EXIT_FAILURE);
    }
  }

  return commands;
}
