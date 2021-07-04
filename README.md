# Process Memory Management

This Program take processes of varying sizes, load them into memory according to each of those rules and swap processes out as needed to create a larger hole.
The processes will come in queue to be loaded in the main memory and main memory has fixed size so,
We have to swap the processes which are in main memory for a long time so that current process gets main memory and swpped process gets updated again in the queue.

### Features of Program

1. Memory Assumption is of size 128MB. 
2. The size of each process will be some whole number of megabytes in the range 1..128.
3. An initial list of processes and their sizes is loaded from a file, which is the first command-line argument. These processes should be loaded into a queue of processes
waiting to be loaded into memory. The second command-line argument is the allocation strategy (algorithm) used: **first, best, next, worst.**
4. If a process needs to be loaded but there is no hole large enough to accommodate it, then processes should be swapped out, one at a time, until there is a hole large enough to hold the process that needs to be loaded.
5. If a process needs to be swapped out, then the process that has been "in memory" the longest should be the one to go.
6. When a process has been swapped out, it goes to the end of the queue of processes
waiting to be swapped in (see next item).
7. Once a process has been swapped out for a third time, we assume that the process has
run to completion and it is not re-queued. Note: not all processes will be swapped out
for a 3^rd^ time.

Conditions That Program will Exit :: 

    1. If all the programs gets loaded into the main memory then no further loading required and exits the program computing the final result.
    2. Else of all programs cannot accomodate then if all programs have entered the min memory thrice, program exits.


### Sample Input 
    A 13
    B 99
    C 2
    D 2
    E 44
    F 32
    G 2
    H 9

### Sample Outputs 

Run Command : ./holes input_file_name algo_type
    
    $ ./holes test1 best

    A loaded, #processes = 1, #holes = 1, %memusage = 10, cumulative %mem = 10
    B loaded, #processes = 2, #holes = 1, %memusage = 88, cumulative %mem = 49
    C loaded, #processes = 3, #holes = 1, %memusage = 89, cumulative %mem = 62
    D loaded, #processes = 4, #holes = 1, %memusage = 91, cumulative %mem = 70
    E loaded, #processes = 3, #holes = 2, %memusage = 38, cumulative %mem = 63
    F loaded, #processes = 4, #holes = 2, %memusage = 63, cumulative %mem = 63
    G loaded, #processes = 5, #holes = 2, %memusage = 64, cumulative %mem = 63
    H loaded, #processes = 6, #holes = 2, %memusage = 71, cumulative %mem = 64
    A loaded, #processes = 7, #holes = 2, %memusage = 81, cumulative %mem = 66
    B loaded, #processes = 1, #holes = 1, %memusage = 77, cumulative %mem = 67
    C loaded, #processes = 2, #holes = 1, %memusage = 79, cumulative %mem = 68
    D loaded, #processes = 3, #holes = 1, %memusage = 80, cumulative %mem = 69
    E loaded, #processes = 3, #holes = 2, %memusage = 38, cumulative %mem = 67
    F loaded, #processes = 4, #holes = 2, %memusage = 63, cumulative %mem = 67
    G loaded, #processes = 5, #holes = 2, %memusage = 64, cumulative %mem = 66
    H loaded, #processes = 6, #holes = 2, %memusage = 71, cumulative %mem = 67
    A loaded, #processes = 7, #holes = 2, %memusage = 81, cumulative %mem = 68
    B loaded, #processes = 2, #holes = 2, %memusage = 88, cumulative %mem = 69
    C loaded, #processes = 3, #holes = 2, %memusage = 89, cumulative %mem = 70
    D loaded, #processes = 4, #holes = 1, %memusage = 91, cumulative %mem = 71
    E loaded, #processes = 3, #holes = 2, %memusage = 38, cumulative %mem = 69
    F loaded, #processes = 4, #holes = 2, %memusage = 63, cumulative %mem = 69
    G loaded, #processes = 5, #holes = 2, %memusage = 64, cumulative %mem = 69
    H loaded, #processes = 6, #holes = 2, %memusage = 71, cumulative %mem = 69
    Total loads = 24, average #processes = 3.88, average #holes = 1.67, cumulative %mem = 69




<br>

### Functionality

Data Structures: 
    
    1. Queue using singly linked list for processes waiting to load.
    2. Singly Linked List Representing the processes in main memory.


Simulation: This respresents 3 process int the main memory executing with base addresses.

    (0MB) |---00000----00000----000000---| (128MB)
              2|P1     60|P2    110|P3  

       Base Address | Process ID
    



##### Queue Functions


1. *void enqueue(process item)* - 
Initializing Queue at the start adding all processes in the queue.
2. *void enqueue_back(process_node *item)*
To add process node at the back of the queue. i.e. at last of linked list.
3. *process_node *dequeue()* - 
Pop front process of the queue and return its pointer
4. *empty()*
To check if queue is empty i.e. no processes.

##### Linked List Functions Representing processes in Main Memory :: 

1. process_node *start --
This is the node which is an Initializing node which has base address as 0 and NULL of linked list in the last will be meaning 128 MB base address.

2. void insert_back(process_node *curr_process) --
Insert at last in the linked list

3. void insert_between(process_node *pr, process_node *suc, process_node *to_be_inserted) --
Given pointers to two process add curr process between them

4. process_node *delete_from_mem() -- Deleting the process in the main memory which has been in for longest which means that which has the minimum order number (int order) will leave.

Structures Used - 

    struct process_node:
        char id;      : ID of the process 
        int size;     : memory needed for the process
        int base_add; : Staring address of Memory Occupation in the Memory 
        int order;    : Order in which it appears in main memory .. Used to get the longest been process in memory

    struct node: 
        process data;       - process is the data.
        struct node *next;  - representing link to next node.

    struct hole:
        process_node *pre;  - prev process in mem of this hole
        process_node *succ; - next process in mem of this hole
        int hole_size;      - size of this hole

    
### Algorithm:

In all 4 algorithms, First we store as many process in intial queue as possible in main memory without creating any holes. Then, if we cannot accomodate the incoming process due to insufficient memory, we start creating hole by swapping the longest in memory process to queue again.

*Main algorithm to create holes in main memory -*

    itr = head of list;
    while (itr != NULL)
    {
        // if its last process then...
        if (itr->next == NULL)
        {
            /* if (its base address + its size is not equal to 128 mb) then there will be hole.
            hole size = 128 - (baseaddress + size)
            */

            if ((itr->data.base_add + itr->data.size) != memory)
            {
                h[i].pre = itr;
                h[i].succ = itr->next;
                h[i].hole_size = memory - (itr->data.base_add + itr->data.size);
                i++;
            }
        }
        else
        {
            /* if not last node or process */
            /* if base address + size != next process' base address then its a hole */

            if ((itr->data.base_add + itr->data.size) != itr->next->data.base_add)
            {
                h[i].pre = itr;
                h[i].succ = itr->next;
                h[i].hole_size = itr->next->data.base_add - (itr->data.base_add + itr->data.size);
                i++;
            }
        }
        itr = itr->next;
    }
    h_size = i;     /* Hole array Size Updated */
    
#### 1. First Fit Algorithm
    
    void first_fit();
Always find hole starting from 0 index in hole array.

#### 2. Next Fit Algorithm
    
    void next_fit();
Always find hole starting from last found index in hole array.
If Swap is made, then starting searching from 0 index in hole array.

#### 3. Best Fit Algorithm
    
    void best_fit();
Always find the hle which is sufficient for the current process but has minimum hole size among all possible.

#### 4. Worst Fit Algorithm
    
    void worst_fit();
Always find the hle which is sufficient for the current process but has maximum hole size among all possible.


###### Freeing all the memory Allocated Dynamically to structs.

    

