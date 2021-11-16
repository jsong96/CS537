/***********************************************************************
* FILENAME :       createExecute.c             
*
* DESCRIPTION :
*       This is a source file for createExecute header file
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
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "createExecute.h"
#include "specGraph.h"

/*
 * Function: command_alignment
 * ----------------------------
 *  
 *   Returns: char ** cmds
 * 
 *   char ** cmds: a list of commandlines 
 *   int numCommands: a number of commandlines in the list
 *
 *   This function reads each commandline and erase extra whitespaces between each words
 *   and returns the aligned version of commands
 */
char **command_alignment(char **cmds, int numCommands)
{
    char *line;
    char *newline;
    int linelen = 0;
    int idx = 0;

    // iterate and skips extra whitespace if there are more than one whitespace between words
    for (int i = 0; i < numCommands; i++)
    {
        line = cmds[i];
        linelen = strlen(line) + 1;
        newline = calloc(linelen, sizeof(char));
        if (newline == NULL)
        {
            fprintf(stderr, "Error: cannot allocate memory for new line.\n");
            exit(EXIT_FAILURE);
        }
        idx = 0;
        int wordflag = 0;
        int spaceflag = 0;

        for (int j = 0; j < linelen; j++)
        {
            if (wordflag == 0 && isspace(line[j]))
            {
                continue;
            }

            if (!isspace(line[j]))
            {
                newline[idx++] = line[j];
                wordflag = 1;
                spaceflag = 0;
            }

            if (wordflag == 1 && isspace(line[j]) && spaceflag == 0)
            {
                if (line[j] == ' ')
                {
                    newline[idx++] = line[j];
                    spaceflag = 1;
                    wordflag = 0;
                }
            }
        }

        // reallocate the memory of the new line
        int newsize = strlen(newline) + 1;
        newline = realloc(newline, sizeof(char) * newsize);
        if (newline == NULL)
        {
            fprintf(stderr, "Error: cannot reallocate memory for new line.\n");
            exit(EXIT_FAILURE);
        }
        free(line);
        line = NULL;
        cmds[i] = newline;
    }
    return cmds;
}

/*
 * Function: split_string
 * ----------------------------
 *   Returns: int argvidx
 * 
 *   char **argv: a char* array to store splitted tokens of char*
 *   char *command: a command to split
 *   int length: the length of the command
 *     
 *   This function splits a commandline string by a space character, stores the splitted char* to argv
 *   and returns the index
 */
int split_string(char **argv, char *command, int length)
{
    // variable initialization
    char *token= NULL;
    int argvidx = 0;
    int splitflag = 1;
    int idx = 0;

    // iterate and splits the string by a space character
    for (int i = 0; i < length; i++)
    {
        if (splitflag)
        {
            token = calloc(length, sizeof(char));
            if (token == NULL)
            {
                fprintf(stderr, "Error: failed to allocate memory.\n");
                exit(EXIT_FAILURE);
            }
            splitflag = 0;
        }
        if (command[i] == ' ')
        {
            argv[argvidx++] = token;
            token = NULL;
            idx = 0;
            splitflag = 1;
        }
        else
        {
            token[idx++] = command[i];
        }
    }
    // add the remaining token
    if (token != NULL)
    {
        argv[argvidx++] = token;
        token = NULL;
    }
    // the last index should be a null pointer
    argv[argvidx++] = NULL;

    return argvidx;
}

/*
 * Function: executeCommand
 * ----------------------------
 *  
 *   cmds: Array of strings for command
 *   numCommands: The number of strings for command
 * 
 *   This function calls split_string for each commandline and 
 *   create a new process by fork() and execute each commandline by execvp()
 */
void executeCommand(char **cmds, int numCommands)
{

    // variable initialization
    pid_t pid;
    int process_status = 0;
    int execErrorStatus = 0;
    int numStrings = 0;
    int len = 0;
    const int BUFFER_SIZE = 4096;

    char **argv = malloc(sizeof(char *) * BUFFER_SIZE);
    if (argv == NULL)
    {
        fprintf(stderr, "Error: failed to allocate memory.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < numCommands; i++)
    {
        len = strlen(cmds[i]) + 1;

        pid = fork();
        /*** CHILD PROCESS ***/
        if (pid == 0)
        {
            numStrings = split_string(argv, cmds[i], len);
            if (numStrings > 0)
            {
                execErrorStatus = execvp(argv[0], argv);
                //if the process fails, print out the error message and exits
                if (execErrorStatus)
                {
                    fprintf(stderr, "Error: Failed to execute %s\n", strerror(errno));
                    fprintf(stderr, "execvp failed at line: %s\n", cmds[i]);
                    exit(EXIT_FAILURE);
                }
            }
        }
        /*** PARENT PROCESS ***/
        else if (pid > 0)
        {
            // parent process waits until the child process finishes
            while (wait(&process_status) != pid)
            {
            }
        }
        else
        {
            fprintf(stderr, "Error: fork function failed to create a child process.\n");
            exit(EXIT_FAILURE);
        }
    }
}