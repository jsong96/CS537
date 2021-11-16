/***********************************************************************
* FILENAME :       specRep.c             
*
* DESCRIPTION :
*       This is the source file for specRep header file.
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
#include "specRep.h"

/*
 * Function: createNodeSingle
 * ----------------------------
 *   Return: Node *n
 * 
 *   tgt: String for target dependency
 *   dps: String for dependencies of the target dependency
 *   cmd: Array of Strings that store the commands for the target dependency
 *   cmdSize: Number of commands for the target dependency
 *
 *   This function initializes a node for the target Dependency and returns the node
 *   to be stored in the GraphNode
 */
Node *createNodeSingle(char *tgt, char *dps, char **cmd, int cmdSize)
{
    /* Initialize new Node */
    Node *n = (Node *)malloc(sizeof(Node));
    if (n == NULL)
    {
        fprintf(stderr, "Error: Failed to allocate memory for Node.\n");
    }
    n->target = tgt;
    n->dependencies = malloc(sizeof(char *));
    n->numDependencies = 0;
    n->command = cmd;
    n->numCommands = cmdSize;

    int length = 1;
    int i = 0;
    char *currentChar = dps;
    char *read = NULL;
    read = strtok(currentChar, " ");

    /* Split dependencies into a separate dependency*/
    while (read != NULL)
    {
        if (n->numDependencies >= length)
        {
            length = length * 2;
            n->dependencies = realloc(n->dependencies, length * sizeof(char *));
        }
        n->dependencies[i] = read;
        i++;
        n->numDependencies++;
        read = strtok(NULL, " ");
    }
    n->numDependencies = i;
    return n;
}

/*
 * Function: createNode
 * ----------------------------
 *   Return: GraphNode *gn
 *
 *   tgt: String for the target dependency
 *   dps: char array for Dependencies of the target dependency
 *   cmd: double pointer of char array that stores the commands for the target dependency
 *   cmdSize: A int for the number of commands for the target dependency
 *
 *   This function initializes GraphNode and returns it to be stored in the graph.
 */
GraphNode *createNode(char *tgt, char *dps, char **cmd, int cmdSize)
{
    Node *node = createNodeSingle(tgt, dps, cmd, cmdSize);

    GraphNode *gn = (GraphNode *)malloc(sizeof(GraphNode));
    gn->node = node;
    gn->children = NULL;
    gn->numChildren = 0;
    gn->sizeChildren = 0;

    return gn;
}
