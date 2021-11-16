/***********************************************************************
* FILENAME :       threads.h             
*
* DESCRIPTION :
*       This is the header file for all the functions for each thread
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon
* CLASS: CS 537
* TERM: FALL 2020
*
**/

#ifndef THREAD
#define THREAD

void *read_input(void *queues);

void *write_to_console(void *queues);

void *lower_to_upper(void *queues);

void *space_to_asterisk(void *queues);

#endif