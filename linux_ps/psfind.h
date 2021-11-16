/***********************************************************************
* FILENAME :       psfind.h             
*
* DESCRIPTION :
*       This is a header file for cs537 version of ps command program.
*
*
* AUTHOR :  Jiwon Song  HyukJoon Yang   
* CLASS: CS 537
* TERM: FALL 2020
*
**/


#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#ifndef PS_FIND
#define PS_FIND

#define MAX_PID 7
#define MAX_NUM_PID 100
#define BUFF_MAX 300
#define LINE_MAX 255

typedef struct Flags{
    int pidflag;
    char * pid;
    int stateflag; 
    int stimeflag;
    int vmemflag; 
    int utimeflag;
    int cmdflag;                              

} flags;

/*
 * Function:  parseArgument 
 * --------------------
 * parses command line argument and set up flags
 *    
 *  argc: number of argument passed
 *  argv: a pointer array which points to each argument passed to the program
 *
 *  returns: flags object with each flag set up according to command line argument
 */
flags * parseArgument(int argc, char * argv[]);

/*
 * Function:  scanDirectory
 * --------------------
 * reads all the files in proc/ directory, 
 *      get process id list that are currently running under user process id
 *
 *
 *  returns: A char double array filled with process id
 *
 */

char ** scanDirectory();

/*
 * Function:  readPidandPrint 
 * --------------------
 * opens a process file under proc/ directory,
 *      extracts information according to the passed flag
 *      and prints out result
 *
 *  flag: a flag pointer to access each flag
 *  pid: a char pointer pointing to process id
 *
 */

void readPidandPrint(flags *flag, char * pid);

/*
 * Function:  findDigit
 * --------------------
 * simply computes a number of digits of passed integer
 *
 *  num: an integer to find digits
 *
 *  returns: a number of digits
 */

int findDigit(int num);


#endif
