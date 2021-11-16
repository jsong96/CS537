/***********************************************************************
* FILENAME :       psfind.c             
*
* DESCRIPTION :
*       This is a source file for psfind header file
*
*
* AUTHOR :  Jiwon Song  HyukJoon Yang   
* CLASS: CS 537
* TERM: FALL 2020
*
**/

#include "psfind.h"

flags *parseArgument(int argc, char *argv[])
{
  // variables for option and PID number
  int opt = 0, i = 0;
  flags *f = malloc(sizeof(flags));
  if (f == NULL)
  {
    fprintf(stderr, "unavailable to allocate memory for flags\n");
    exit(EXIT_FAILURE);
  }

  /* Declare flags */
  int pidflag = 0, stateflag = 0, stimeflag = 0, vmemflag = 0; // flags with default value False
  int utimeflag = 1, cmdflag = 1;                              // flags with default value True

  int flagtrue = 0; // Boolean if a flag  is changed to be true

  /* Read the user-command from the console */
  while ((opt = getopt(argc, argv, "p:S::s::U::v::c::")) != -1)
  {

    // turn off error message
    opterr = 0;

    /* Switch to options read from the command-line arguments */
    switch (opt)
    {

    case 'p':
      /* Check if the argument is valid */
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
        /* Change pid flag true if the argument is true. Otherwise, exit */
        if (valid)
        {
          f->pid = optarg;
          pidflag = 1;
          flagtrue = 1;
        }
        else
        {
          fprintf(stderr, "error: process ID list syntax error\n");
          exit(EXIT_FAILURE);
        }
      }
      /* Print error if no argument for p option is received*/
      else
      {
        fprintf(stderr, "error: list of process IDs must follow -p\n");
        exit(EXIT_FAILURE);
      }
      break;

    case 'S':
      /* Check for no display option but no effect  */
      if ((optarg != NULL) && (strcmp(optarg, "-") == 0))
      {
        stimeflag = 0;
        flagtrue = 1;
      }
      /* Check for syntax error */
      else if (optarg != NULL)
      {
        fprintf(stderr, "error: option syntax error\n");
        exit(EXIT_FAILURE);
      }
      /* Changes stime flag */
      else
      {
        stimeflag = 1;
        flagtrue = 1;
      }
      break;

    case 's':
      /* Check for no display option but no effect */
      if ((optarg != NULL) && (strcmp(optarg, "-") == 0))
      {
        stateflag = 0;
        flagtrue = 1;
      }
      /* Check for syntax error */
      else if (optarg != NULL)
      {
        fprintf(stderr, "error: option syntax error\n");
        exit(EXIT_FAILURE);
      }
      /* Change state flag */
      else
      {
        stateflag = 1;
        flagtrue = 1;
      }
      break;

    case 'v':
      /* Check for no display option but no effect */
      if ((optarg != NULL) && (strcmp(optarg, "-") == 0))
      {
        vmemflag = 0;
        flagtrue = 1;
      }
      /* Check for syntax error*/
      else if (optarg != NULL)
      {
        fprintf(stderr, "error: option syntax error\n");
        exit(EXIT_FAILURE);
      }
      /* Change vmem flag*/
      else
      {
        vmemflag = 1;
        flagtrue = 1;
      }
      break;

    case 'U':
      /* Check for no display option*/
      if ((optarg != NULL) && (strcmp(optarg, "-") == 0))
      {
        utimeflag = 0;
        flagtrue = 1;
      }
      /* Check for syntax error*/
      else if (optarg != NULL)
      {
        fprintf(stderr, "error: option syntax error\n");
        exit(EXIT_FAILURE);
      }
      /*Chanve utime flag*/
      else
      {
        utimeflag = 1;
        flagtrue = 1;
      }
      break;

    case 'c':
      /* Check for no display option */
      if ((optarg != NULL) && (strcmp(optarg, "-") == 0))
      {
        cmdflag = 0;
        flagtrue = 1;
      }
      /* Check for syntax error */
      else if (optarg != NULL)
      {
        fprintf(stderr, "error: option syntax error\n");
        exit(EXIT_FAILURE);
      }
      /* Change cmd flag */
      else
      {
        cmdflag = 1;
        flagtrue = 1;
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
  /* Exit for cases when there is no option and there are strings of characters */
  if (flagtrue == 0 && argc > 1)
  {
    fprintf(stderr, "Error: unsupported option\n");
    exit(EXIT_FAILURE);
  }

  /* Update flag */
  f->pidflag = pidflag;
  f->stateflag = stateflag;
  f->stimeflag = stimeflag;
  f->vmemflag = vmemflag;
  f->utimeflag = utimeflag;
  f->cmdflag = cmdflag;

  return f;
}

int findDigit(int num)
{
  int digit = 0;
  do
  {
    digit++;
    num /= 10;
  } while (num != 0);

  return digit;
}

void readPidandPrint(flags *flag, char *pid)
{

  /* Arrays of characters that will be used as a path of a file to be open */
  const char *proc = "/proc/";
  const char *stat = "/stat";
  const char *statm = "/statm";
  const char *cmdline = "/cmdline";

  /* Variables that stores the data related to the pid */
  char state;
  char *userTime;
  char *stime;
  char *vmem;

  /* Array of characters that store path to stat file of pid */
  char *path = calloc((sizeof(proc) + sizeof(pid) + sizeof(stat)), sizeof(char));

  if (path == NULL)
  {
    fprintf(stderr, "unavailable to allocate memory for readPidandPrint()\n");
    exit(EXIT_FAILURE);
  }
  sprintf(path, "%s%s%s", proc, pid, stat);

  /**** OPENING STAT FILE ******/
  FILE *fp;

  /* Check if such file exists */
  if ((fp = fopen(path, "r")) != NULL)
  {
    printf("%s: ", pid);
  }
  else
  {
    exit(EXIT_FAILURE);
  }

  char line[LINE_MAX]; // Array of charaters that read a line of file
  int count = 0;       // The number of spaces counted

  /* Read characters from the file */
  while (fscanf(fp, "%s", line) == 1)
  {
    count++;

    /* Read state if state flag is true */
    if (count == 3 && flag->stateflag)
    {
      state = line[0];
      printf("%c ", state);
      memset(line, 0, LINE_MAX);
    }

    /* Read user time if utime flag is true */
    if (count == 14 && flag->utimeflag)
    {
      userTime = line;
      printf("utime=%s ", userTime);
      memset(line, 0, LINE_MAX);
    }

    /* Read system time if stime flage is true */
    if (count == 15 && flag->stimeflag)
    {
      stime = line;
      printf("stime=%s ", stime);
      memset(line, 0, LINE_MAX);
    }
  }

  /**** FREE MEMORY ****/
  free(path);
  path = NULL;
  /**** CLOSE FILE ****/
  fclose(fp);

  /* Array of characters that store path to statm file of pid */
  path = calloc((sizeof(proc) + sizeof(pid) + sizeof(statm)), sizeof(char));
  if (path == NULL)
  {
    fprintf(stderr, "unavailable to allocate memory for readPidandPrint()\n");
    exit(EXIT_FAILURE);
  }
  sprintf(path, "%s%s%s", proc, pid, statm);

  /**** OPENING STATM FILE ****/
  fp = fopen(path, "r");

  /* Read virtual memory used if vmemflag is true */
  if (fscanf(fp, "%s", line) == 1 && flag->vmemflag)
  {
    vmem = line;
    printf("vmem=%s ", vmem);
    memset(line, 0, LINE_MAX);
  }

  /**** CLOSE FILE ****/
  fclose(fp);

  /**** FREE MEMORY ****/
  free(path);
  path = NULL;

  /* Read command-line if cmdflag is true */
  if (flag->cmdflag)
  {
    /* Array of characters that store path to cmdline file of pid */
    path = calloc((sizeof(proc) + sizeof(pid) + sizeof(cmdline)), sizeof(char));
    if (path == NULL)
    {
      fprintf(stderr, "unavailable to allocate memory for readPidandPrint()\n");
      exit(EXIT_FAILURE);
    }
    sprintf(path, "%s%s%s", proc, pid, cmdline);

    /**** OPENING CMDLINE FILE ****/
    fp = fopen(path, "r");

    char word; // Character read from the file
    int i = 0; // Index of a character array that will store the variable word

    /* Read non-null characters in the file*/
    while ((word = fgetc(fp)) != EOF)
    {
      if (word != '\0')
      {
        line[i] = word;
        i++;
      }
    }
    printf("[%s]", line);
    memset(line, 0, LINE_MAX);

    /**** CLOSE FILE ****/
    fclose(fp);

    /**** FREE MEMORY ****/
    free(path);
    path = NULL;
  }

  printf("\n");
}

char **scanDirectory()
{

  /** set path variables **/
  const char *proc = "/proc/";
  const char *status = "/status";

  /** allocate double char array to list process ids **/
  char **pid_list = (char **)calloc(MAX_NUM_PID, sizeof(char *));
  int list_count = 0;

  /** check memory allocation **/
  if (pid_list == NULL)
  {
    fprintf(stderr, "error: cannot allocate memory\n");
    exit(EXIT_FAILURE);
  }

  /** allocate memory for each row **/
  for (int i = 0; i < MAX_NUM_PID; i++)
  {
    pid_list[i] = (char *)calloc(MAX_PID, sizeof(char));
  }

  // get user id, getuid is always successful, no need for return value check
  int id = getuid();
  // find number of digits
  int digit = findDigit(id);

  // allocate heap memory for user id according to the number of digits
  char *userId = calloc(sizeof(digit), sizeof(char));

  // check memory allocation
  if (userId == NULL)
  {
    fprintf(stderr, "error: cannot allocate memory\n");
    exit(EXIT_FAILURE);
  }
  sprintf(userId, "%d", id);

  /** open proc/ directory **/
  DIR *directory = opendir(proc);
  struct dirent *file;

  if (directory == NULL)
  {
    fprintf(stderr, "error opening virtual file system directory\n");
    exit(EXIT_FAILURE);
  }

  /** read each file in proc/ directory **/
  while ((file = readdir(directory)))
  {
    /** check whether a file's name is process id **/
    int is_process = 1;
    int size = strlen(file->d_name);
    for (int i = 0; i < size; i++)
    {
      if (!isdigit(file->d_name[i]))
      {
        is_process = 0;
        break;
      }
    }

    /** if the file's name is a process id, open the file **/
    if (is_process)
    {

      char *path = calloc((sizeof(proc) + sizeof(file->d_name) + sizeof(status)), sizeof(char));
      /** combine char arrays to form a path to the status file **/
      sprintf(path, "%s%s%s", proc, file->d_name, status);

      FILE *fp = fopen(path, "r");
      char word[BUFF_MAX];
      char *pid = NULL;
      int userProcess = 0, getpid = 0;

      /** scan each line in the file **/
      while ((fscanf(fp, "%s", word)) == 1)
      {

        if (userProcess)
        {
          if (strcmp(word, userId) == 0)
          {
            /** reallocate memory size if size does not match **/
            if (digit < MAX_PID)
            {
              pid_list[list_count] = realloc(pid_list[list_count], digit);
            }
            /** add process id to the list **/
            pid_list[list_count++] = pid;
          }
          else
          {
            /** free memory for unused pid **/
            free(pid);
            pid = NULL;
          }
          break;
        }
        else if (getpid)
        {
          int digit = strlen(word);
          /** store process id of each file **/
          pid = (char *)calloc(digit, sizeof(char));

          if (pid == NULL)
          {
            fprintf(stderr, "error: cannot allocate memory\n");
            exit(EXIT_FAILURE);
          }

          sprintf(pid, "%s", word);
          getpid = 0;
        }
        else
        {
          if (strcmp(word, "Pid:") == 0)
          {
            getpid = 1;
          }
          if (strcmp(word, "Uid:") == 0)
          {
            userProcess = 1;
          }
        }
      }

      /**** FREE MEMORY ****/
      free(path);
      path = NULL;
      /** close the file **/
      fclose(fp);
      /** reset buffer **/
      memset(word, 0, BUFF_MAX);
    }
  }

  /** close proc/ directory **/
  if (closedir(directory) != 0)
  {
    fprintf(stderr, "error: cannot close the virtual file system directory\n");
    exit(EXIT_FAILURE);
  }

  /** free heap allocated memory **/
  free(userId);
  userId = NULL;

  /** return process id list **/
  return pid_list;
}
