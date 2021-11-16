# 537ps
## About this program
This program is the first program assignment of CS 537 FALL 2020.
The purpose was to create a homebrew version of linux ps program.
This program supports six options (p, s, U, S, v, c)

## contributors
- Jiwon Song 
- HyukJoon Yang

### List of C library we are using:
1. #include <stdio.h>
2. #include <sys/types.h>
3. #include <dirent.h>
4. #include <errno.h>
5. #include <string.h>
6. #include <stdlib.h>
7. #include <unistd.h>
8. #include <ctype.h>


### Functions
typedef struct flags -> contains flag variables for each option

flags * parseArgument(int argc, char * argv[]);

void openProcFile(flags* flag);

void scanDirectory();

void readPidandPrint(flags *flag, char * pid);

int findDigit(int num);

### How to compile
Download files: main.c Makefile, psfind.h, psfind.c 
To compile, type *make*, it will generate 537ps file.
To delete object files, simply type *make clean*

### How to run
To run, type *./537ps* with options 

### options
-p <pid>
Display process information only for the process whose number is pid. It does not matter if the specified process is owned by the current user. If this option is not present then display information for all processes of the current user (and only of the current user). Our program only supports one -p option.
  
-s
Display the single-character state information about the process. This option defaults to be false, so if it is not present, do not display this information. -s- is valid but has no effect.

-U
Display the amount of user time consumed by this process. This option defaults to be true, so if it is not present, then this information is displayed. -U- turns this option off.

-S
Display the amount of system time consumed so far by this process. This option defaults to be false, so if it is not present, then this information is not displayed. -S- is valid but has no effect.

-v
Display the amount of virtual memory currently being used (in pages) by this program. This option defaults to be false, so if it is not present, then this information is not displayed. -v- is valid but has no effect.

-c
Display the command-line that started this program. This option defaults to be true, so if it is not present, then this information is displayed. -c- turns this option off.


