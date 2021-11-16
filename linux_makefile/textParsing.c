/***********************************************************************
* FILENAME :      textParsing.c            
*
* DESCRIPTION :
*       This is the source file for parsing a makefile
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon 
* CLASS: CS 537
* TERM: FALL 2020
*
**/
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "specGraph.h"
#include "specRep.h"
#include "textParsing.h"

/*
 * Function: parse_makefile
 * ----------------------------
 *
 *   graph: Graph object that requires nodes to be connected each other
 *
 *   This function reads a makefile and parse the file into a graph
 */
Graph *parse_makefile()
{
  FILE *fp;
  fp = fopen("makefile", "r");
  if (fp != NULL)
  {
  }
  else
  {
    fp = fopen("Makefile", "r");
    if (fp == NULL)
    {
      fprintf(stderr, "Error: makefile not found.\n");
      exit(EXIT_FAILURE);
    }
  }

  /*** Variables ***/
  char c = '\0';
  char *target = NULL;
  char *dependency = NULL;
  char **commandList = NULL;
  char *commandline = NULL;
  const int BUFFER_SIZE = 4096;
  int lineidx = 1;
  int commandidx = 0;
  int listSize = 1;
  int listIdx = 0;
  int targetlen = 0;
  int tmplen = 0;
  int depenlen = 0;
  int targetidx = 0;
  int idx = 0;
  char *line = NULL;

  /**** FLAGS ****/
  int targetFound = 0;
  int reset = 0;
  int blankline = 1;
  int errorflag = 0;
  int newline = 1;

  /*** Graph initialization ***/
  Graph *graph = createGraph();
  GraphNode *v;

  while ((c = fgetc(fp)) != EOF)
  {
    // newline initialization
    if (newline)
    {
      // remove remaining line characters
      if (line != NULL)
      {
        free(line);
        line = NULL;
      }
      line = calloc(BUFFER_SIZE, sizeof(char));
      if (line == NULL)
      {
        fprintf(stderr, "Error: Failed to allocate memory for line buffer.\n");
        exit(EXIT_FAILURE);
      }
      newline = 0;
      idx = 0;
    }

    // linesize management
    if (idx >= BUFFER_SIZE)
    {
      fprintf(stderr, "%d: <Error: a line is longer than buffer size>: %s\n", lineidx, line);
      exit(EXIT_FAILURE);
    }

    // commentline management
    if (c == '#')
    {
      while (c != '\n')
      {
        c = fgetc(fp);
      }
      newline = 1;
      lineidx++;
      continue;
    }

    // newline management
    if (c == '\n' && targetFound == 0)
    {
      // missing colon management
      if (idx > 0 && line[0] != '\0')
      {
        fprintf(stderr, "%d: <missing colon>: %s\n", lineidx, line);
        exit(EXIT_FAILURE);
      }
      else
      {
        newline = 1;
        lineidx++;
        continue;
      }
    }

    /*** COMMAND LINE PARSING ***/
    if (targetFound)
    {
      // keep running until it finds another target or the file ends
      while (c != EOF)
      {
        // skip any blank lines
        while (c == '\n' && c != EOF)
        {
          c = fgetc(fp);
          lineidx++;
        }
        if (c == EOF)
        {
          break;
        }
        commandline = calloc(BUFFER_SIZE, sizeof(char));
        if (commandline == NULL)
        {
          fprintf(stderr, "Error: cannot allocated memory for commandline.\n");
          exit(EXIT_FAILURE);
        }
        commandidx = 0;
        errorflag = 0;

        //check if a line starts with a tab
        if (c != '\t')
        {
          if (c != '\n')
          {
            errorflag = 1;
          }
        }
        while (c != '\n' || c != EOF)
        {
          if (c == '\n' || c == EOF)
          {
            break;
          }
          if (c == '\0' && commandidx != 0)
          {
            fprintf(stderr, "%d: <a null character in a line>: %s\n", lineidx, commandline);
            exit(EXIT_FAILURE);
          }

          // reached another target line
          if (c == ':')
          {
            reset = 1;
            break;
          }

          // skip any comments
          if (c == '#')
          {
            while (c != '\n')
            {
              c = fgetc(fp);
            }
          }

          // skip the first tab character
          if (commandidx == 0 && c == '\t')
          {
            c = fgetc(fp);
            continue;
          }
          commandline[commandidx++] = c;
          c = fgetc(fp);
        }

        // commandline format checking
        if (errorflag && reset == 0 && commandidx > 1)
        {
          blankline = 1;
          for (int i = 0; i < commandidx; i++)
          {
            if (isspace(commandline[commandidx]) == 0)
            {
              blankline = 0;
            }
          }

          if (blankline == 0)
          {
            fprintf(stderr, "%d: <a commandline should always start with a tab character> %s\n", lineidx, commandline);
            exit(EXIT_FAILURE);
          }
          errorflag = 0;
        }

        if (reset == 0)
        {
          // store only valid commandlines
          if (commandline[0] != '\0' && commandidx > 0)
          {
            if (listIdx >= listSize)
            {
              listSize = listSize * 2;
              commandList = realloc(commandList, sizeof(char *) * listSize);
              if (commandList == NULL)
              {
                fprintf(stderr, "Error: failed to allocate memory for commandList.\n");
                exit(EXIT_FAILURE);
              }
            }
            commandList[listIdx++] = commandline;
            commandline = NULL;
          }
          // free unnecessary memory usages
          else
          {
            free(commandline);
            commandline = NULL;
          }
          lineidx++;
        }
        else
        {
          // create a node and add it to the graph
          v = createNode(target, dependency, commandList, listIdx);
          addNode(graph, v);

          // save the target
          if (commandline != NULL)
          {
            tmplen = strlen(commandline) + 1;
            targetidx = 0;
            target = (char *)calloc(tmplen, sizeof(char));
            if (target == NULL)
            {
              fprintf(stderr, "Error: cannot allocated memory for target line.\n");
              exit(EXIT_FAILURE);
            }

            for (int i = 0; i < tmplen; i++)
            {
              if (isspace(commandline[i]))
              {
                break;
              }
              // null character handling
              if (commandline[i] == '\0' && i > 0 && i < tmplen - 1)
              {
                fprintf(stderr, "%d: <Null character in a line>: %s\n", lineidx, commandline);
                exit(EXIT_FAILURE);
              }
              target[targetidx++] = commandline[i];
            }
            targetlen = strlen(target) + 1;
            if (targetlen < tmplen)
            {
              target = realloc(target, sizeof(char) * targetlen);
              if (target == NULL)
              {
                fprintf(stderr, "Error: failed to reallocate memory for target.\n");
                exit(EXIT_FAILURE);
              }
            }
          }
          else
          {
            target = NULL;
          }
          // reset buffers and flags
          dependency = NULL;
          commandList = NULL;
          commandline = NULL;
          reset = 0;
          break;
        }
      }
    }

    /*** TARGET AND DEPENDENCY LINE PARSING***/
    if (c == ':')
    {

      targetFound = 1;
      targetidx = 0;
      tmplen = 0;

      // store target and trim the extra whitespaces
      if (target == NULL)
      {
        tmplen = strlen(line) + 1;
        target = (char *)calloc(tmplen, sizeof(char));
        if (target == NULL)
        {
          fprintf(stderr, "Error: cannot allocated memory for target line.\n");
          exit(EXIT_FAILURE);
        }
        for (int i = 0; i < tmplen; i++)
        {
          if (isspace(line[i]))
          {
            break;
          }
          // null character handling
          if (line[i] == '\0' && i > 0 && i < tmplen - 1)
          {
            fprintf(stderr, "%d: <Null character in a line>: %s\n", lineidx, line);
            exit(EXIT_FAILURE);
          }
          target[targetidx++] = line[i];
        }
        targetlen = strlen(target) + 1;

        // compare the size of two buffers and reallocate memory
        if (targetlen < tmplen)
        {
          target = realloc(target, sizeof(char) * targetlen);
          if (target == NULL)
          {
            fprintf(stderr, "Error: failed to reallocate memory for target.\n");
            exit(EXIT_FAILURE);
          }
        }

        line = NULL;
        line = calloc(BUFFER_SIZE, sizeof(char));
        idx = 0;
      }

      // store the remaining dependency line
      while (c != '\n' || c != EOF)
      {
        c = fgetc(fp);
        if (c == '\n' || c == EOF)
        {
          break;
        }

        // skip the comments
        if (c == '#')
        {
          break;
        }

        // null character handling
        if (c == '\0' && idx != 0)
        {
          fprintf(stderr, "%d: <Null character in a line>: %s\n", lineidx, commandline);
          exit(EXIT_FAILURE);
        }
        line[idx++] = c;
      }
      depenlen = strlen(line) + 1;
      // dependency line initialization
      dependency = (char *)calloc(depenlen, sizeof(char));
      if (dependency == NULL)
      {
        fprintf(stderr, "Error: cannot allocated memory for dependency line.\n");
        exit(EXIT_FAILURE);
      }

      dependency = line;

      // reset line buffer
      line = NULL;
      idx = 0;

      // commandlist initialization
      commandList = (char **)malloc(sizeof(char *) * listSize);
      if (commandList == NULL)
      {
        fprintf(stderr, "Error: cannot allocated memory for commandList.\n");
        exit(EXIT_FAILURE);
      }
      listIdx = 0;
      newline = 1;
      lineidx++;
      continue;
    }

    if (c == EOF)
    {
      break;
    }
    // store each word to a line buffer
    line[idx++] = c;
  }

  // add the last line of the makefile to the graph
  if (target != NULL)
  {
    if (dependency != NULL)
    {
      if (commandList != NULL)
      {
        v = createNode(target, dependency, commandList, listIdx);
        addNode(graph, v);
      }
    }
    target = NULL;
    dependency = NULL;
    commandList = NULL;
  }

  // if there are extra words
  if (line != NULL)
  {
    free(line);
    line = NULL;
  }
  fclose(fp);
  addConnection(graph);
  return graph;
}
