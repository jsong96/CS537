# 537make 
simple 537 version of make command.

### Purposes
1. Learn about creating new processes, loading new programs, and waiting for processes to complete in Linux.
2. Get experience with poorly formed input and handling the errors gracefully. (Defensive programming.)
3. Learning a few more useful Linux kernel calls, such as fstat().


## Author
 - Jiwon Song  
 - Hyukjoon Yang  

## modules to think about

- Build specification graph: This module has functions for both building the graph that represents the dependences between build specifications, and traversing the graph in a bottom-up order to evaluate the specifications (effectively a post-order traversal).
- Text parsing: This module contains functions that help you parse lines in the makefile. It splits a line into an array of strings, checking whether the line begins with a tab or regular character, and filters out blank lines.
- Build specification representation: This module contains the basic build specification abstraction. It allows you to create, update, and access a build specification.
- Process creation and program execution: This module is responsible for running each build command in a new process, waiting for its completion, and getting the return code.

### C library functions we have to use
1. fstat()
- identical to stat(), it checks whether the target is out of date or not
- call this function on an open file

2. fork()
- for creating a new process

3. execvp()
- to load a new program into the child process
- this is a variant of exec() command

4. stat()
- to check the modification time on a file 
- call this function on a file path


### The modular design 
1. textParsing
    
    char** read_line(File *fp)
        creates a Graph object
        reads each word in each line in a makefile
        handles cases such as not reading comment lines and detecting invalid line format
        parses target, dependency and command lines into a Graph node and passes it to the Graph object
        

2. createExecute
    
    void executeCommand(char **cmds, int numCommands)
        it uses fork() and execvp(). fork() is used to make another child process to run and execvp() to execute commands that are passed on

    int split_string(char **argv, char*command, int length)
        it splits each command line by space delimeter and stores them into a char** array in a correct format to pass the array to executeCommand()  

    char **command_alignment(char **cmds, int numCommands)
        it removes extra whitespace between each word in a command line

    

3. specGraph (Graph)
    void createGraph()
        Initialize the graph with default values
            

    void createNode(char *line) 
        Node = createNode(char target); 
        Sort the command
        Update the node
        Insert the node into the nodes


    void traverseGraph (char target)
        search the target in the nodes
        get dependencies in array of nodes
        while (*nodes)
            executeCommand(nodes)
            traverseGraph (nodes);
            nodes++;


4. specRep (Node)
    Node *createNodeSingle(char *tgt, char *dps, char **cmd, int cmdSize)
        called by createNode() method, it tokenizes dependencies by a space delimeter, stores the dependencies into a node object and returns the node object  

    GraphNode *createNode(char *tgt, char *dps, char **cmd, int cmdSize)
        calls createNode() to create a Node object and forms a GraphNode with the Node object


5. main
    calls methods from the modules to read and execute commands in a makefile
    returns 0 if terminates successfully
   
