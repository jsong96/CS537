/***********************************************************************
* FILENAME :       input.c             
*
* DESCRIPTION :
*       This source file parses option arguments and find unique pids in passed file
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon
* CLASS: CS 537
* TERM: FALL 2020
*
**/
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include "input.h"


/*
 * Function: parse_options
 * ----------------------------
 *  
 *   Returns: flags *f
 * 
 *   int argc :  number of arguments 
 *   char *const argv[]: argument vectors
 *
 *   This function parses arguments passed to this program and 
 *     returns information with flag struct
 *    
 */
flags *parse_options(int argc, char *const argv[])
{
  int opt = 0;
  int i = 0;
  const int default_pagesize = 4096;
  const int default_memsize = 1048576;

  flags *f = malloc(sizeof(flags));
  if (f == NULL)
  {
    fprintf(stderr, "Error: failed to allocate memory for flags.\n");
    exit(EXIT_FAILURE);
  }
  f->pagesize = default_pagesize;
  f->memsize = default_memsize;
  f->filename = NULL;

  while ((opt = getopt(argc, argv, "p:m:")) != -1)
  {
    opterr = 0;

    switch (opt)
    {
    case 'p':
      if (optarg != NULL)
      {
        int valid = 1, size = strlen(optarg);
        for (i = 0; i < size; i++)
        {
          if (!isdigit(optarg[i]))
          {
            valid = 0;
          }
        }
        if (valid)
        {
          f->pagesize = atoi(optarg);
          if ((f->pagesize % 2) != 0)
          {
            fprintf(stderr, "Error: page size is not power of 2\n");
            exit(EXIT_FAILURE);
          }
        }
      }
      else
      {
        fprintf(stderr, "Error: pagesize not provided after -p\n");
        exit(EXIT_FAILURE);
      }
      break;

    case 'm':
      if (optarg != NULL)
      {
        int valid = 1, size = strlen(optarg);
        for (i = 0; i < size; i++)
        {
          if (!isdigit(optarg[i]))
          {
            valid = 0;
          }
        }

        if (valid)
        {
          f->memsize = atoi(optarg) * default_memsize;
        }
      }
      else
      {
        fprintf(stderr, "Error: memsize not provided after -m\n");
        exit(EXIT_FAILURE);
      }
      break;

    case '?':
      /* Exit for any invalid option */
      fprintf(stderr, "Error: unsupported option\n");
      exit(EXIT_FAILURE);
      break;

    default:
      fprintf(stderr, "Error: unsupported option\n");
      break;
    }
  }

  if (optind < argc)
  {
    f->filename = argv[optind];
  }

  return f;
}


/*
 * Function: check_num
 * ----------------------------
 *  
 *   Returns: 0 or 1
 * 
 *   char *num:  char array of numbers
 *
 *   This function simply checks if the char* num
 *    only contains number digits
 */
int check_num(char *num)
{
  int size = strlen(num);
  for (int i = 0; i < size; i++)
  {
    if (isdigit(num[i]) == 0)
    {
      if (isspace(num[i]) == 0)
      {
        return 0;
      }
    }
  }
  return 1;
}

/*
 * Function: makePidnode
 * ----------------------------
 *  
 *   Returns: pidNode *node
 * 
 *
 *   This function simply initializes a pidNode and returns
 */
pidNode *makePidnode()
{
  pidNode *node = malloc(sizeof(pidNode));
  if (node == NULL)
  {
    fprintf(stderr, "Error: failed to allocate memory.\n");
    exit(EXIT_FAILURE);
  }
  node->pid = 0;
  node->curpos = 0;
  node->start_reference = 0;
  node->finishing_reference = 0;
  node->done = 0;
  node->block = 0;

  return node;
}

/*
 * Function: getPidandVpn_test
 * ----------------------------
 *  
 *   Returns: parseNode *tmp
 * 
 *   char *line: a char array representation of a line in a file
 *
 *   This function simply parses pid and vpn from a char line 
 */
parseNode *getPidandVpn_test(char *line)
{
  parseNode *tmp = malloc(sizeof(parseNode));
  if (tmp == NULL)
  {
    fprintf(stderr, "Error: failed to allocate memory.\n");
    exit(EXIT_FAILURE);
  }
  int size = strlen(line) + 1;
  char *token = NULL;
  int pid = 0;
  int vpn = 0;
  char buff[size];
  for (int i = 0; i < size; i++)
  {
    buff[i] = line[i];
  }
  token = strtok(buff, " ");
  if (check_num(token))
  {
    pid = atoi(token);
  }
  else
  {
    fprintf(stderr, "<Error> wrong format:%s\n", token);
    exit(EXIT_FAILURE);
  }

  token = strtok(NULL, " ");
  if (check_num(token))
  {
    vpn = atoi(token);
  }
  else
  {
    fprintf(stderr, "<Error> wrong format:%s\n", token);
    exit(EXIT_FAILURE);
  }

  tmp->pid = pid;
  tmp->vpn = vpn;
  return tmp;
}


/*
 * Function: realloc_list
 * ----------------------------
 *  
 *   Returns: pidNode ** list
 * 
 *   int size: size to use in realloc
 *   pidNode ** list: list to realloc 
 *
 *   This function simply realloc list's size and returns
 */
pidNode **realloc_list(int size, pidNode **list)
{
  list = realloc(list, sizeof(pidNode *) * size);
  if (list == NULL)
  {
    fprintf(stderr, "Error: failed to reallocate memory.\n");
    exit(EXIT_FAILURE);
  }

  return list;
}

/*
 * Function: get_pidlist
 * ----------------------------
 *  
 *   Returns: pidList * pid_list
 * 
 *   char *filename: a filename to open
 *
 *   This function reads a file with given name and
 *     find all pids with their starting and finishing reference locations
 */
pidList *get_pidlist(char *filename)
{
  char *line = NULL;
  size_t line_len = 0;
  FILE *fp = fopen(filename, "r");
  if (fp == NULL)
  {
    fprintf(stderr, "Error: file does not exist.\n");
    exit(EXIT_FAILURE);
  }

  pidList *pid_list = malloc(sizeof(pidList));
  if (pid_list == NULL)
  {
    fprintf(stderr, "Error: failed to allocate memory.\n");
    exit(EXIT_FAILURE);
  }
  pid_list->TMR = 0;
  pid_list->size = 1;
  pid_list->list = malloc(sizeof(pidNode *) * pid_list->size);
  if (pid_list->list == NULL)
  {
    fprintf(stderr, "Error: failed to allocate memory.\n");
    exit(EXIT_FAILURE);
  }

  long int curpid = 0;
  long int nextpid = 0;
  long int curpos = 0;
  long int tmp = 0;
  int newpid = 1;
  int idx = 0;
  pidNode *node = NULL;
  parseNode *line_parse = NULL;

  while (1)
  {
    // starting location: (here)3 1
    curpos = ftell(fp);

    // read next line
    if (getline(&line, &line_len, fp) == EOF)
    {
      break;
    }
    // count line number
    pid_list->TMR++;
    // parse line
    line_parse = getPidandVpn_test(line);

    //node initialization only one time
    if (newpid)
    {
      newpid = 0;
      node = makePidnode();
      curpid = line_parse->pid;
      node->pid = curpid;
      node->start_reference = curpos;
      tmp = ftell(fp);
      continue;
    }

    nextpid = line_parse->pid;
    if (curpid != nextpid)
    {
      curpid = nextpid;
      node->finishing_reference = tmp;
      tmp = curpos;
      curpos = ftell(fp);

      pid_list->list[idx++] = node;
      if (pid_list->size <= idx)
      {
        pid_list->size = pid_list->size * 2;
        pid_list->list = realloc_list(pid_list->size, pid_list->list);
      }
      // make new node
      node = NULL;
      node = makePidnode();
      node->pid = curpid;
      node->start_reference = tmp;
    }
    else
    {
      tmp = curpos;
    }
  }

  if (node != NULL)
  {
    node->finishing_reference = tmp;
    pid_list->list[idx++] = node;
  }

  fclose(fp);
  return pid_list;
}

/*
 * Function: filter_pidlist
 * ----------------------------
 *  
 *   Returns: pidList * pid_list
 * 
 *   pidList *list: list to filter 
 *
 *   This function reads a list of pids and filter out
 *      duplicates and rerecord their starting and finishing locations
 */
pidList *filter_pidlist(pidList *list)
{
  int validrange = 0;
  pidList *uniquepidList = malloc(sizeof(pidList));
  if (uniquepidList == NULL)
  {
    fprintf(stderr, "Error: unable to allocate memory.\n");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < list->size; i++)
  {
    if (list->list[i] != NULL)
    {
      validrange = i + 1;
    }
  }

  uniquepidList->list = malloc(sizeof(pidNode *) * validrange);
  if (uniquepidList->list == NULL)
  {
    fprintf(stderr, "Error: unable to allocate memory.\n");
    exit(EXIT_FAILURE);
  }
  uniquepidList->size = 0;
  uniquepidList->TMR = list->TMR;

  pidNode *node;
  pidNode *new;
  pidNode *tmp;
  pidNode *check;
  int already_checked = 0;
  long int curpid = 0;
  for (int i = 0; i < validrange; i++)
  {
    node = list->list[i];
    for (int idx = 0; idx < uniquepidList->size; idx++)
    {
      check = uniquepidList->list[idx];
      if (check->pid == node->pid)
      {
        already_checked = 1;
        break;
      }
    }
    if (!already_checked)
    {
      new = makePidnode();
      new->pid = node->pid;
      new->start_reference = node->start_reference;
      new->finishing_reference = node->finishing_reference;
      curpid = node->pid;

      for (int j = i; j < validrange; j++)
      {
        tmp = list->list[j];
        if (tmp->pid == curpid)
        {
          if (tmp->start_reference <= new->start_reference)
          {
            new->start_reference = tmp->start_reference;
          }

          if (tmp->finishing_reference > new->finishing_reference)
          {
            new->finishing_reference = tmp->finishing_reference;
          }
        }
      }
      new->curpos = new->start_reference;
      uniquepidList->list[uniquepidList->size++] = new;
      new = NULL;
    }
    else
    {
      already_checked = 0;
    }
  }

  // free unnecessary memory
  for (int i = 0; i < list->size; i++)
  {
    free(list->list[i]);
  }
  free(list);
  return uniquepidList;
}
