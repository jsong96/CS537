/***********************************************************************
* FILENAME :       main.c             
*
* DESCRIPTION :
*       This is a source file that runs cs537 version of ps command program.
*
*
* AUTHOR :  Jiwon Song  HyukJoon Yang   
* CLASS: CS 537
* TERM: FALL 2020
*
**/

#include "psfind.h"


/*
 * Function: main
 * --------------------
 * This is the main function that receives user inputs and processes outputs
 *
 *  argc: number of arguments passed to the program
 *  argv: a pointer array that points to each argument passed to the program
 *  
 *
 *  returns: 0 for successful exit, exit with EXIT_FAILURE for any errors
 */

int main(int argc, char *argv[])
{
    
  // parse command line arguments
  flags * flag = parseArgument(argc, argv);

  
  // if the user specifies process id
  if (flag->pidflag)
  {
      readPidandPrint(flag, flag->pid);
  }
  // if the user does not provide process id
  else {
      char ** pidList = scanDirectory();     
      
      for(int i = 0; i < MAX_NUM_PID; i++)
      {
      
          if(pidList[i][0] == 0)
          {
              break;
          }
          readPidandPrint(flag, pidList[i]);
      }
      
      // free heap allocated memory
      for(int i = 0; i < MAX_NUM_PID; i++)   
      {
         free(pidList[i]);
      }
      // free double pointer
      free(pidList);
  }
  
  // free heap allocated memory
  free(flag);
  return 0;
}
