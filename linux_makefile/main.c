/***********************************************************************
* FILENAME :       main.c             
*
* DESCRIPTION :
*       This is the main source file that executes makefile
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
#include "textParsing.h"
#include "specGraph.h"

/*
 * Function: createGraph
 * ----------------------------
 *  Return: 0
 *
 *  This function creates a directed graph of dependencies in makefile. 
 *  It will not create a graph if there is a cycle of dependencies or 
 *  if there is a violation of makefile format.
 *  If there is no argument, it will run the makefile by executing targets
 *  in the makefile by post-order traversal of the graph. If there is an
 *  argument, it will run the makefile by executing the target dependency
 *  by post-order traversal of the graph.
 */
int main(int argc, char *argv[])
{
  Graph *graph = parse_makefile();

  int result = detectCycle(graph);
  if (result == 1)
  {
    fprintf(stderr, "Error: there is a cycle in the dependency chain.\n");
    exit(EXIT_FAILURE);
  }
  else
  {
    char *test = argv[1];

    Commands *cmds = traverse(graph, test);

    for (int i = 0; i < cmds->cmdsIndex; i++)
    {
      cmds->cmds[i]->cmd = command_alignment(cmds->cmds[i]->cmd, cmds->cmds[i]->cmdSize);
    }

    for (int i = 0; i < cmds->cmdsIndex; i++)
    {
      executeCommand(cmds->cmds[i]->cmd, cmds->cmds[i]->cmdSize);
    }

    for (int i = 0; i < cmds->cmdsIndex; i++)
    {
      free(cmds->cmds[i]);
      cmds->cmds[i] = NULL;
    }

    free(cmds);
    cmds = NULL;
  }

  fclose(stdin);
  fclose(stdout);
  fclose(stderr);

  return 0;
}
