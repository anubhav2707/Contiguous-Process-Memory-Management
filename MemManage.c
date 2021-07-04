#include <stdio.h>     /* Input/Output */
#include <stdlib.h>    /* General Utilities */
#include <unistd.h>    /* Symbolic Constants */
#include <string.h>    /* Strings */
#include <math.h>      /* Mathematics Library */

typedef struct PROCESS
{
    char id;
    int size;
    int base_add; /* Staring address of Memory Occupation in Hole */
    int order;
} process;

typedef struct node
{
    process data;
    struct node *next;
} process_node;

typedef struct HOLE
{
    process_node *pre;
    process_node *succ;
    int hole_size;
} hole;

/* Constant Variables Needed */
const int memory = 128;
int num_processes = 0;
int three_times_loaded = 0;
process *plist;

/* Queue Implementation for Processes waiting to load */
process_node *front = NULL;
process_node *rear = NULL;

void enqueue(process item)
{
    process_node *nptr = (process_node *)malloc(sizeof(process_node));
    nptr->data.base_add = item.base_add;
    nptr->data.id = item.id;
    nptr->data.size = item.size;
    nptr->data.order = -1;

    nptr->next = NULL;

    if (rear == NULL)
    {
        front = nptr;
        rear = nptr;
    }
    else
    {
        rear->next = nptr;
        rear = rear->next;
    }
}

void enqueue_back(process_node *item)
{
    if (rear == NULL)
    {
        front = item;
        rear = item;
    }
    else
    {
        rear->next = item;
        rear = rear->next;
    }
}

process_node *dequeue()
{
    if (front == rear)
    {
        process_node *temp;
        temp = front;
        front = rear = NULL;
        temp->next = NULL;
        return temp;
    }

    if (front == NULL)
    {
        printf("\nNo Process Left to Load.\n");
        return NULL;
    }
    else
    {
        process_node *temp;
        temp = front;
        front = front->next;
        temp->next = NULL;
        return temp;
    }
}

int empty()
{
    if ((front == NULL) && (rear == NULL))
        return 1;
    else
        return 0;
}

/* Singly linked List Representing main main memory */

process_node *start = NULL;
int sequencing = 1; /* To Get Longest in main Memory */

void insert_back(process_node *curr_process)
{
    curr_process->data.order = sequencing++;
    process_node *head = start;
    while (head->next != NULL)
    {
        head = head->next;
    }
    curr_process->data.base_add = head->data.base_add;
    head->next = curr_process;
    return;
}

void insert_between(process_node *pr, process_node *suc, process_node *to_be_inserted)
{
    to_be_inserted->data.order = sequencing++;
    if (suc == NULL)
    {
        to_be_inserted->next = NULL;
        pr->next = to_be_inserted;
        return;
    }
    to_be_inserted->next = suc;
    pr->next = to_be_inserted;
    return;
}

/* Delete which is for longest in the main memory */

process_node *delete_from_mem()
{
    /* Basically Swapping Operation */

    process_node *itr = start;

    int min_in_list = 100000000;
    process_node *min_node = NULL;

    while (itr)
    {
        if ((itr->data.order > 0) && (itr->data.order < min_in_list))
        {
            min_in_list = itr->data.order;
            min_node = itr;
        }
        itr = itr->next;
    }

    if (min_node != start)
    {
        itr = start;

        while (itr->next != min_node)
        {
            itr = itr->next;
        }
        process_node *del = itr->next;
        if (del->next == NULL)
        {
            itr->next = NULL;
            return del;
        }
        else
        {
            itr->next = del->next;
            del->next = NULL;
            return del;
        }
    }
    return NULL;
}

int min(int a, int b)
{
    return (a > b) ? b : a;
}

/* Helper Functions Done */

void first_fit(char count_loaded[])
{
    /* Initialising Start Node representing 0 base address */
    process_node *temp = (process_node *)malloc(sizeof(process_node));
    temp->data.size = 0;
    temp->data.id = '-';
    temp->data.order = -1;
    temp->data.base_add = 0; /* Most Important */
    temp->next = NULL;

    start = temp;

    /* Initialising Queue with all processes in order they occur */
    int j = 0;
    for (j = 0; j < num_processes; j++)
    {
        enqueue(plist[j]);
    }

    /* Variables for Final Computation */
    int avg_processes = 0, avg_holes = 0, total_loads = 0;
    float cumulative = 0;

    /* Initializing an array of holes which would be changing while we progress */
    hole *h = (hole *)malloc(1000000 * sizeof(hole));

    /* Last Swap Variable is for checking if there was a swap done in the last iteration */
    int last_swap = 0, h_size = 0;

    /*
    Conditions That Program will Exit :: 
        1. If all the programs gets loaded into the main memory then no further loading required and exits the program computing the final result.
        2. Else of all programs cannot accomodate then if all programs have entered the min memory thrice, program exits.
    */

    while ((!empty()) && (three_times_loaded < num_processes))
    {
        int sz = 0;
        if (start->next == NULL) /* Memory is empty */
        {
            /* Directly load the process */
            process_node *del = dequeue();
            insert_back(del);
            sz += del->data.size;
            del->data.base_add = start->data.base_add;

            int mem = sz;
            float _mem_percent = ((mem * (1.0) * 100) / (1.0 * memory));
            total_loads++;
            cumulative += _mem_percent;
            float cum = ((cumulative * 1.0) / (1.0 * total_loads));

            count_loaded[((int)(del->data.id))]++;
            /* if the current process is loaded for three times update the number of process that have been ran 3 times */
            if (count_loaded[((int)(del->data.id))] == 3)
            {
                three_times_loaded++;
            }

            char c = '%';
            avg_processes += 1;
            avg_holes += 1;
            printf("%c loaded, #processes = 1, #holes = 1, %cmemusage = %.0f, cumulative %cmem = %.0f\n", del->data.id, c, round(_mem_percent), c, round(cum));

            /* Hole count will be one now */
            h[0].pre = del;
            h[0].succ = NULL;
            h[0].hole_size = memory - del->data.size;
            h_size = 1;
            last_swap = 0;
        }
        else
        {
            int i = 0;
            int processes = 0, holes = 0; /* Number of processes and holes in current iteration */

            process_node *itr = start;

            if (last_swap == 1) /* if Swap has happened then we will have to make the new hole array as there might be different holes added */
            {
                /* Making H array free from other pointers */
                if (h_size != 0)
                {
                    for (i = 0; i < h_size; i++)
                    {
                        h[i].pre = NULL;
                        h[i].succ = NULL;
                    }
                    h_size = 0;
                }

                /* finding holes using formula described in README */
                i = 0;
                while (itr != NULL)
                {
                    if (itr->next == NULL)
                    {
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

                h_size = i;
            }
            /* Boolean loaded to check if there is any hole which can accomodate the current process in any hole */
            int loaded = 0, load_idx = i;
            process_node *del = front;

            /* First fit method of finding the hole by always starting from 0 and checking all holes and breaking when found one */
            for (i = 0; i < h_size; i++)
            {
                if (h[i].hole_size >= del->data.size)
                {
                    loaded = 1;
                    load_idx = i;
                    break;
                }
            }

            /* if we didn't found any hole sufficient then we need swapping */
            if (!loaded)
            {
                /* ...Swapping Needed... */
                process_node *swap = delete_from_mem();
                swap->data.base_add = -1;
                enqueue_back(swap);
                last_swap = 1;
            }
            else
            {
                /* ...Computing and Printing State... */
                del = NULL;
                del = dequeue(); /* Take process out from queue */

                /* Update its base address as the current after being uploaded in main memory */

                del->data.base_add = h[load_idx].pre->data.base_add + h[load_idx].pre->data.size;
                insert_between(h[load_idx].pre, h[load_idx].succ, del); /* inserting */
                last_swap = 0;

                /* After adding this process to main mem there would be new hole sizes so updating hole array again */
                for (i = 0; i < h_size; i++)
                {
                    h[i].pre = NULL;
                    h[i].succ = NULL;
                }

                /* finding holes using formula described in README */
                i = 0;
                itr = start;
                while (itr != NULL)
                {
                    if (itr->next == NULL)
                    {
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
                holes = i;
                h_size = i;

                /* Counting Number of Processes in the main mem right now */
                itr = start;
                int sz = 0;
                while (itr != NULL)
                {
                    processes++;
                    sz += itr->data.size;
                    itr = itr->next;
                }
                processes--; /* start node is not a process */

                char c = '%';
                int mem = sz;
                float _mem_percent = round((mem * (1.0) * 100) / (1.0 * memory));
                total_loads++;
                cumulative += _mem_percent;
                float cum = ((cumulative * 1.0) / (1.0 * total_loads));
                count_loaded[((int)(del->data.id))]++;
                if (count_loaded[((int)(del->data.id))] == 3)
                {
                    three_times_loaded++;
                }

                avg_processes += processes;
                avg_holes += holes;
                printf("%c loaded, #processes = %d, #holes = %d, %cmemusage = %.0f, cumulative %cmem = %.0f\n", del->data.id, processes, holes, c, round(_mem_percent), c, round(cum));
            }
        }
    }
    /* Freeing holes array memory */
    for (j = 0; j < 1000000; j++)
    {
        h[j].pre = NULL;
        h[j].succ = NULL;
    }
    free(h);

    /* Final computation of the algorithm */
    char c = '%';
    printf("Total loads = %d, average #processes = %.2f, average #holes = %.2f, cumulative %cmem = %.0f\n", total_loads, ((avg_processes * 1.0) / (total_loads * 1.0)), ((avg_holes * 1.0) / (total_loads * 1.0)), c, round((cumulative * 1.0) / (total_loads * 1.0)));
    return;
}

void next_fit(char count_loaded[])
{
    /* Initialising Start Node representing 0 base address */
    process_node *temp = (process_node *)malloc(sizeof(process_node));
    temp->data.size = 0;
    temp->data.id = '-';
    temp->data.order = -1;
    temp->data.base_add = 0; /* Most Important */
    temp->next = NULL;

    start = temp;

    /* Initialising Queue with all processes in order they occur */
    int j = 0;
    for (j = 0; j < num_processes; j++)
    {
        enqueue(plist[j]);
    }
    /* Variables for Final Computation */
    /* last_idx is used for the last index from which we have to start checking for accurate holes */
    int avg_processes = 0, avg_holes = 0, total_loads = 0, last_idx = -1;
    float cumulative = 0;

    /* Initializing an array of holes which would be changing while we progress */
    hole *h = (hole *)malloc(1000000 * sizeof(hole));

    /* Last Swap Variable is for checking if there was a swap done in the last iteration */
    int last_swap = 0, h_size = 0;

    /*
    Conditions That Program will Exit :: 
        1. If all the programs gets loaded into the main memory then no further loading required and exits the program computing the final result.
        2. Else of all programs cannot accomodate then if all programs have entered the min memory thrice, program exits.
    */

    while ((!empty()) && (three_times_loaded < num_processes))
    {
        int sz = 0;
        if (start->next == NULL)
        {
            /* Directly load the process */
            process_node *del = dequeue();
            insert_back(del);
            sz += del->data.size;
            del->data.base_add = start->data.base_add;

            int mem = sz;
            float _mem_percent = ((mem * (1.0) * 100) / (1.0 * memory));
            total_loads++;
            cumulative += _mem_percent;
            float cum = ((cumulative * 1.0) / (1.0 * total_loads));

            count_loaded[((int)(del->data.id))]++;
            /* if the current process is loaded for three times update the number of process that have been ran 3 times */
            if (count_loaded[((int)(del->data.id))] == 3)
            {
                three_times_loaded++;
            }

            char c = '%';
            avg_processes += 1;
            avg_holes += 1;
            printf("%c loaded, #processes = 1, #holes = 1, %cmemusage = %.0f, cumulative %cmem = %.0f\n", del->data.id, c, round(_mem_percent), c, round(cum));

            /* Hole count will be one now */
            h[0].pre = del;
            h[0].succ = NULL;
            h[0].hole_size = memory - del->data.size;
            h_size = 1;
            last_swap = 0;
            /* last_idx will be updated to 0 */
            last_idx = 0;
        }
        else
        {
            int i = 0;
            int processes = 0, holes = 0;

            process_node *itr = start;

            if (last_swap) /* if swap is made last iteration then recompte holes array */
            {
                /* Making H array free from other pointers */
                if (h_size != 0)
                {
                    for (i = 0; i < h_size; i++)
                    {
                        h[i].pre = NULL;
                        h[i].succ = NULL;
                    }
                    h_size = 0;
                }

                /* finding holes using formula described in README */
                i = 0;
                while (itr != NULL)
                {
                    if (itr->next == NULL)
                    {
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

                h_size = i;
            }

            /* Boolean loaded to check if there is any hole which can accomodate the current process in any hole */
            int loaded = 0;
            process_node *del = front;

            /* Next fit method of starting from the last Index */
            if (last_idx != -1)
            {
                /* we will check from last_idx to last and then circularly up to last_idx to find holes */

                for (i = last_idx; i < h_size; i++)
                {
                    if (h[i].hole_size >= del->data.size)
                    {
                        last_idx = i;
                        loaded = 1;
                        break;
                    }
                }
                /* if no hole found till now */
                /* suppose last process size was equal to hole then so last_idx would be incremented but if h_size is smaller than that */
                if (!loaded)
                {
                    for (i = 0; i < min(last_idx, h_size); i++)
                    {
                        if (h[i].hole_size >= del->data.size)
                        {
                            last_idx = i;
                            loaded = 1;
                            break;
                        }
                    }
                }
            }
            /* If current process not loaded then we have to swap it */
            /* if swapping is done then last idx will be  = 0 */
            if (!loaded)
            {
                /* ...Swapping Needed... */
                process_node *swap = delete_from_mem();
                swap->data.base_add = -1;
                enqueue_back(swap);
                last_swap = 1;
                last_idx = 0;
            }
            else
            {
                /* ...Computing and Printing State... */
                del = NULL;
                del = dequeue();

                /* Update its base address as the current after being uploaded in main memory */

                del->data.base_add = h[last_idx].pre->data.base_add + h[last_idx].pre->data.size;
                insert_between(h[last_idx].pre, h[last_idx].succ, del);
                last_swap = 0;

                for (i = 0; i < h_size; i++)
                {
                    h[i].pre = NULL;
                    h[i].succ = NULL;
                }

                /* Computing hole array after addition of the process to main mem */
                int prev_h_size = h_size;
                i = 0;
                itr = start;
                while (itr != NULL)
                {
                    if (itr->next == NULL)
                    {
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
                holes = i;
                h_size = i;

                /* Suppose the number of holes changes then it should start from beginning */
                /* It will happen if hole size is equal to the process size being accomodated */
                if (h_size != prev_h_size)
                {
                    last_idx++;
                }

                itr = start;
                int sz = 0;
                while (itr != NULL)
                {
                    processes++;
                    sz += itr->data.size;
                    itr = itr->next;
                }
                processes--; /* Start node is not a process */

                char c = '%';
                int mem = sz;
                float _mem_percent = round((mem * (1.0) * 100) / (1.0 * memory));
                total_loads++;
                cumulative += _mem_percent;
                float cum = ((cumulative * 1.0) / (1.0 * total_loads));
                count_loaded[((int)(del->data.id))]++;
                if (count_loaded[((int)(del->data.id))] == 3)
                {
                    three_times_loaded++;
                }

                avg_processes += processes;
                avg_holes += holes;
                printf("%c loaded, #processes = %d, #holes = %d, %cmemusage = %.0f, cumulative %cmem = %.0f\n", del->data.id, processes, holes, c, round(_mem_percent), c, round(cum));
            }
        }
    }
    /* Freeing Hole array memory */
    for (j = 0; j < 1000000; j++)
    {
        h[j].pre = NULL;
        h[j].succ = NULL;
    }
    free(h);
    /* Final Computation of the all loads */
    char c = '%';
    printf("Total loads = %d, average #processes = %.2f, average #holes = %.2f, cumulative %cmem = %.0f\n", total_loads, ((avg_processes * 1.0) / (total_loads * 1.0)), ((avg_holes * 1.0) / (total_loads * 1.0)), c, round((cumulative * 1.0) / (total_loads * 1.0)));
    return;
}

void best_fit(char count_loaded[])
{
    /* Initialising Start Node representing 0 base address */
    process_node *temp = (process_node *)malloc(sizeof(process_node));
    temp->data.size = 0;
    temp->data.id = '-';
    temp->data.order = -1;
    temp->data.base_add = 0; /* Most Important */
    temp->next = NULL;

    start = temp;

    /* Initialising Queue with all processes in order they occur */
    int j = 0;
    for (j = 0; j < num_processes; j++)
    {
        enqueue(plist[j]);
    }

    /* Variables for Final Computation */
    int avg_processes = 0, avg_holes = 0, total_loads = 0;
    float cumulative = 0;

    /* Initializing an array of holes which would be changing while we progress */
    hole *h = (hole *)malloc(1000000 * sizeof(hole));

    /* Last Swap Variable is for checking if there was a swap done in the last iteration */
    int last_swap = 0, h_size = 0;

    /*
    Conditions That Program will Exit :: 
        1. If all the programs gets loaded into the main memory then no further loading required and exits the program computing the final result.
        2. Else of all programs cannot accomodate then if all programs have entered the min memory thrice, program exits.
    */

    while ((!empty()) && (three_times_loaded < num_processes))
    {

        int sz = 0;
        if (start->next == NULL) /* if memory is empty */
        {
            /* Directly Load the process */
            process_node *del = dequeue();
            insert_back(del);
            sz += del->data.size;
            del->data.base_add = start->data.base_add;

            int mem = sz;
            float _mem_percent = ((mem * (1.0) * 100) / (1.0 * memory));
            total_loads++;
            cumulative += _mem_percent;
            float cum = ((cumulative * 1.0) / (1.0 * total_loads));
            count_loaded[((int)(del->data.id))]++;
            /* if the current process is loaded for three times update the number of process that have been ran 3 times */
            if (count_loaded[((int)(del->data.id))] == 3)
            {
                three_times_loaded++;
            }

            char c = '%';
            avg_processes += 1;
            avg_holes += 1;
            printf("%c loaded, #processes = 1, #holes = 1, %cmemusage = %.0f, cumulative %cmem = %.0f\n", del->data.id, c, round(_mem_percent), c, round(cum));

            /* Update hole array as only one hole will be there */
            h[0].pre = del;
            h[0].succ = NULL;
            h[0].hole_size = memory - del->data.size;
            h_size = 1;
            last_swap = 0;
        }
        else
        {
            int i = 0;
            int processes = 0, holes = 0;

            process_node *itr = start;

            if (last_swap) /* if swap is made last iteration then recompte holes array */
            {
                /* Making H array free from other pointers */
                if (h_size != 0)
                {
                    for (i = 0; i < h_size; i++)
                    {
                        h[i].pre = NULL;
                        h[i].succ = NULL;
                    }
                    h_size = 0;
                }

                /* finding holes using formula described in README */
                i = 0;
                while (itr != NULL)
                {
                    if (itr->next == NULL)
                    {
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
                h_size = i;
            }

            int loaded = 0;
            process_node *del = front;

            /* BEST FIT ALGORITHM to find the minimum size hole which can accomodate incoming process */
            int min_hole = 1000000, min_idx = -1;
            for (i = 0; i < h_size; i++)
            {
                if (h[i].hole_size >= del->data.size)
                {
                    loaded = 1;
                    if (h[i].hole_size < min_hole)
                    {
                        min_hole = h[i].hole_size;
                        min_idx = i;
                    }
                }
            }

            /* If can't be loaded Swapping Needed */
            if (!loaded)
            {
                /* ...Swapping Needed... */
                process_node *swap = delete_from_mem();
                swap->data.base_add = -1;
                enqueue_back(swap);
                last_swap = 1;
            }
            else
            {
                /* ...Computing and Printing State... */
                del = NULL;
                del = dequeue(); /* Take process out from Queue */

                if (min_idx != -1)
                {
                    /* Update its base address as the current after being uploaded in main memory */
                    del->data.base_add = h[min_idx].pre->data.base_add + h[min_idx].pre->data.size;
                    insert_between(h[min_idx].pre, h[min_idx].succ, del);
                    last_swap = 0;
                }

                for (i = 0; i < h_size; i++)
                {
                    h[i].pre = NULL;
                    h[i].succ = NULL;
                }

                /* Computing Hole Array after process is added */
                i = 0;
                itr = start;
                while (itr != NULL)
                {
                    if (itr->next == NULL)
                    {
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
                holes = i;
                h_size = i;

                /* Counting processes */
                itr = start;
                int sz = 0;
                while (itr != NULL)
                {
                    processes++;
                    sz += itr->data.size;
                    itr = itr->next;
                }
                processes--; /* Start node is not a process */

                char c = '%';
                int mem = sz;
                float _mem_percent = round((mem * (1.0) * 100) / (1.0 * memory));
                total_loads++;
                cumulative += _mem_percent;
                float cum = ((cumulative * 1.0) / (1.0 * total_loads));
                count_loaded[((int)(del->data.id))]++;
                if (count_loaded[((int)(del->data.id))] == 3)
                {
                    three_times_loaded++;
                }

                avg_processes += processes;
                avg_holes += holes;
                printf("%c loaded, #processes = %d, #holes = %d, %cmemusage = %.0f, cumulative %cmem = %.0f\n", del->data.id, processes, holes, c, round(_mem_percent), c, round(cum));
            }
        }
    }
    /* Freeing Holes array Memory */
    for (j = 0; j < 1000000; j++)
    {
        h[j].pre = NULL;
        h[j].succ = NULL;
    }
    free(h);

    /* Final computation of the algorithm */
    char c = '%';
    printf("Total loads = %d, average #processes = %.2f, average #holes = %.2f, cumulative %cmem = %.0f\n", total_loads, ((avg_processes * 1.0) / (total_loads * 1.0)), ((avg_holes * 1.0) / (total_loads * 1.0)), c, round((cumulative * 1.0) / (total_loads * 1.0)));
    return;
}

void worst_fit(char count_loaded[])
{
    process_node *temp = (process_node *)malloc(sizeof(process_node));
    temp->data.size = 0;
    temp->data.id = '-';
    temp->data.order = -1;
    temp->data.base_add = 0; /* Most Important */
    temp->next = NULL;

    start = temp;

    int j = 0;
    for (j = 0; j < num_processes; j++)
    {
        enqueue(plist[j]);
    }

    int avg_processes = 0, avg_holes = 0, total_loads = 0;
    float cumulative = 0;

    hole *h = (hole *)malloc(1000000 * sizeof(hole));
    int last_swap = 0, h_size = 0;

    while ((!empty()) && (three_times_loaded < num_processes))
    {

        int sz = 0;
        if (start->next == NULL)
        {
            process_node *del = dequeue();
            insert_back(del);
            sz += del->data.size;
            del->data.base_add = start->data.base_add;

            int mem = sz;
            float _mem_percent = ((mem * (1.0) * 100) / (1.0 * memory));
            total_loads++;
            cumulative += _mem_percent;
            float cum = ((cumulative * 1.0) / (1.0 * total_loads));
            count_loaded[((int)(del->data.id))]++;
            /* if the current process is loaded for three times update the number of process that have been ran 3 times */
            if (count_loaded[((int)(del->data.id))] == 3)
            {
                three_times_loaded++;
            }

            char c = '%';
            avg_processes += 1;
            avg_holes += 1;
            printf("%c loaded, #processes = 1, #holes = 1, %cmemusage = %.0f, cumulative %cmem = %.0f\n", del->data.id, c, round(_mem_percent), c, round(cum));

            h[0].pre = del;
            h[0].succ = NULL;
            h[0].hole_size = memory - del->data.size;
            h_size = 1;
            last_swap = 0;
        }
        else
        {
            int i = 0;
            int processes = 0, holes = 0;

            process_node *itr = start;

            if (last_swap) /* if swap has been made in last iteration */
            {
                /* Making hole array free from pointers and recomputing */
                if (h_size != 0)
                {
                    for (i = 0; i < h_size; i++)
                    {
                        h[i].pre = NULL;
                        h[i].succ = NULL;
                    }
                    h_size = 0;
                }

                i = 0;
                while (itr != NULL)
                {
                    if (itr->next == NULL)
                    {
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

                h_size = i;
            }

            int loaded = 0;
            process_node *del = front;

            /* WORST FIT ALGORITHM to take the hole which has maximum size which can accomodate incoming Process */
            int max_hole = -1000000, max_idx = -1;
            for (i = 0; i < h_size; i++)
            {
                if (h[i].hole_size >= del->data.size)
                {
                    loaded = 1;
                    if (h[i].hole_size > max_hole)
                    {
                        max_hole = h[i].hole_size;
                        max_idx = i;
                    }
                }
            }

            if (!loaded)
            {
                /* ...Swapping Needed... */
                process_node *swap = delete_from_mem();
                swap->data.base_add = -1;
                enqueue_back(swap);
                last_swap = 1;
            }
            else
            {
                /* ...Printing State... */
                del = NULL;
                del = dequeue();

                if (max_idx != -1)
                {
                    /* Updating Base Address */
                    del->data.base_add = h[max_idx].pre->data.base_add + h[max_idx].pre->data.size;
                    insert_between(h[max_idx].pre, h[max_idx].succ, del);
                    last_swap = 0;
                }

                for (i = 0; i < h_size; i++)
                {
                    h[i].pre = NULL;
                    h[i].succ = NULL;
                }

                /* Computing Hole Array after process is added to main memory */
                i = 0;
                itr = start;
                while (itr != NULL)
                {
                    if (itr->next == NULL)
                    {
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
                holes = i;
                h_size = i;

                itr = start;
                int sz = 0;
                while (itr != NULL)
                {
                    processes++;
                    sz += itr->data.size;
                    itr = itr->next;
                }
                processes--; /* start is not a process */

                char c = '%';
                int mem = sz;
                float _mem_percent = round((mem * (1.0) * 100) / (1.0 * memory));
                total_loads++;
                cumulative += _mem_percent;
                float cum = ((cumulative * 1.0) / (1.0 * total_loads));
                count_loaded[((int)(del->data.id))]++;
                if (count_loaded[((int)(del->data.id))] == 3)
                {
                    three_times_loaded++;
                }

                avg_processes += processes;
                avg_holes += holes;
                printf("%c loaded, #processes = %d, #holes = %d, %cmemusage = %.0f, cumulative %cmem = %.0f\n", del->data.id, processes, holes, c, round(_mem_percent), c, round(cum));
            }
        }
    }
    /* Freeing Hole array memory */
    for (j = 0; j < 1000000; j++)
    {
        h[j].pre = NULL;
        h[j].succ = NULL;
    }
    free(h);

    /* Final computation */
    char c = '%';
    printf("Total loads = %d, average #processes = %.2f, average #holes = %.2f, cumulative %cmem = %.0f\n", total_loads, ((avg_processes * 1.0) / (total_loads * 1.0)), ((avg_holes * 1.0) / (total_loads * 1.0)), c, round((cumulative * 1.0) / (total_loads * 1.0)));
    return;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Too Few Arguments.\n");
        return 0;
    }

    char *filename = (char *)malloc(10000);
    char *method = (char *)malloc(10000);
    plist = (process *)malloc(10000 * sizeof(process));
    strcpy(filename, argv[1]);
    strcpy(method, argv[2]);

    char c;
    int i = 0, temp = 0;

    FILE *fptr;
    fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        printf("Couldn't Find or Open the File.\n");
        return 0;
    }
    for (c = getc(fptr); c != EOF; c = getc(fptr))
    {
        if (c == '\n')
            temp++;
    }

    fclose(fptr);

    fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        free(method);
        free(filename);
        free(plist);
        printf("Couldn't Find or Open the File.\n");
        return 0;
    }
    num_processes = temp;
    for (i = 0; i < num_processes; i++)
    {
        fscanf(fptr, "%c %d", &plist[i].id, &plist[i].size);
        c = fgetc(fptr);
        plist[i].base_add = -1;
    }

    fclose(fptr);

    if (strcmp(method, "first") == 0)
    {
        char count_loaded[256];
        memset(count_loaded, 0, sizeof(count_loaded));
        first_fit(count_loaded);
    }
    else if (strcmp(method, "next") == 0)
    {
        char count_loaded[256];
        memset(count_loaded, 0, sizeof(count_loaded));
        next_fit(count_loaded);
    }
    else if (strcmp(method, "best") == 0)
    {
        char count_loaded[256];
        memset(count_loaded, 0, sizeof(count_loaded));
        best_fit(count_loaded);
    }
    else if (strcmp(method, "worst") == 0)
    {
        char count_loaded[256];
        memset(count_loaded, 0, sizeof(count_loaded));
        worst_fit(count_loaded);
    }
    else
    {
        printf("Not a Valid Hole (Memory) Management Method.\n");
        free(filename);
        free(method);
        free(plist);
        return 0;
    }

    /* If Queue not Empty */
    if (!empty())
    {
        while (!empty())
        {
            process_node *node = dequeue();
            free(node);
        }
    }
    /* If Main Memory List not Empty */
    process_node *itr = start;
    process_node *delit = NULL;

    while (itr != NULL)
    {
        delit = itr;
        itr = itr->next;
        free(delit);
    }

    free(filename);
    free(method);
    free(plist);
    return 0;
}