# CS 537 P2 by Jiwon Song and Hyukjoon Yang

### Description

This program creates 4 different threads running at the same time when the program is running. 
In one thread, it will read the strings stored in a text file and send to another thread by using Queue. 
The second thread will receive strings from the queue sent from the first thread and will replace spaces by '*'. 
Then it will send to next thread by using queue. 
The third thread will replace strings into uppercase letters. 
The last thread read strings and print to the console. 

We have 4 different modules for this program. 
One of modules is queue.c. The functions of this module are to create a Queue that stores at maximum 10 strings, 
to enqueue and dequeue strings by using two semaphores and to print statistics of the queue by calling functions 
from the different module 'statistics.c'.

The module 'threads.c' functions to create functionality of threads created in the program. The functions for the
 threads are reading inputs, replacing space to asterisk, replacing lowercase to uppercase, and writing outputs.

The module 'statistics.c' is to record the number of strings that are enqueued and dequeued from a queue and 
the time consumed for the operations. 

The module 'main.c' is the main module that runs the program. It initializes the threads and queues. 
After all operations are done, it will print statistics for each queue by calling functions from queue.c. 
Then it will free all allocated memory used in this program.
