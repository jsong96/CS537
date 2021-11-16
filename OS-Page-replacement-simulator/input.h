/***********************************************************************
* FILENAME :       input.h             
*
* DESCRIPTION :
*       This is a header file for input source file
*
*
* AUTHOR :  Jiwon Song  Hyukjoon Yang  
* NetID :   jsong99        hyang348
* CSLogin:  jis            hyukjoon
* CLASS: CS 537
* TERM: FALL 2020
*
**/

#ifndef INPUT
#define INPUT

typedef struct pidnode
{
  long int pid;
  long int curpos;
  long int start_reference;
  long int finishing_reference;
  int done;
  int block;
} pidNode;

typedef struct pidlist
{
  pidNode **list;
  int size;
  int TMR;
} pidList;

typedef struct Flags
{
  int pagesize;
  int memsize;
  char *filename;
} flags;

typedef struct parsenode
{
  long int pid;
  long int vpn;
} parseNode;

pidNode **realloc_list(int size, pidNode **list);

parseNode *getPidandVpn_test(char *line);

pidNode *makePidnode();

flags *parse_options(int argc, char *const argv[]);

pidList *get_pidlist(char *filename);

pidList *filter_pidlist(pidList *list);

int check_num(char *vpn);

#endif

