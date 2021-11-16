/***********************************************************************
* FILENAME :       createExecute.h             
*
* DESCRIPTION :
*       This is the header file for createExecute.c
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon
* CLASS: CS 537
* TERM: FALL 2020
*
**/

#ifndef CREATE_EXECUTE
#define CREATE_EXECUTE

void executeCommand(char **cmds, int numCommands);

int split_string(char **argv, char *command, int length);

char **command_alignment(char **cmds, int numCommands);

#endif