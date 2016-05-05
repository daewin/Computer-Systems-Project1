/********************************************************
 * Simulation Function for Project 1
 * Subject: COMP30023 Computer Systems
 * Author: Daewin SV Lingam
 * Student ID: 679182
 * Date: 9/4/2016
 ********************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "simulation.h"


int
main(int argc, char* argv[])
{
    // Simulation variable
    time_t time = 0;
    int processes_complete = 0;
    
    // Simulation Options variable declaration
    char* process_file_name = NULL;
    char* algorithm = NULL;
    memsize_t memsize = 0;
    
    // Current process declaration
    process_data_t *current_process = NULL;
    
    // Linked list creation for process and memory
    struct list *process_list = create_list("process");
    struct list *next_processes_list = create_list("process");
    struct list *free_memory_list = create_list("memory");
    struct list *allocated_memory_list = create_list("memory");
    
    // Multi-Level Feedback Queue Creation
    struct list *q1 = create_list("q1");
    struct list *q2 = create_list("q2");
    struct list *q3 = create_list("q3");
    
    // Process the options
    getOptions(argc, argv, &process_file_name, &algorithm, &memsize);
    
    // Process the input file and make a struct of the process data 
    processes_t *processes = process(process_file_name, memsize);
    
    // Memory is empty for first run, thus free list is all of memory
    free_memory_initial(free_memory_list, memsize, memsize);
    

    if(strcmp(algorithm, FCFS) == 0)
    {
        /***********************************
        ** First Come First Served 
        ***********************************/
        
        // Insert the processes into a queue
        insert_processes_to_list(processes, process_list);
        
        // We run the first process at time=0            
        run_process(free_memory_list, allocated_memory_list, 
                            process_list, &current_process, time);
        
        printf("time %d, %d running, numprocesses=%d, ",
                (int)time, current_process->process_id,
                    get_list_size(allocated_memory_list));
                          
        printf("numholes=%d, memusage=%d%%\n", get_list_size(free_memory_list), 
                memory_usage(get_used_memory(allocated_memory_list), memsize));
                          
        while(!processes_complete)
        {
            // While all the processes have not been completed, keep looping
            
            if(is_empty(process_list) && current_process->job_time == 0)
            {
                // All processes including current have completed running.
                processes_complete = 1;
                printf("time %d, simulation finished.", (int)time);
                
            } 
                else if(current_process->job_time == 0)
            {                                            
                // Current process has completed running.  
                // Free the completed process from memory.

                free_process_from_memory(free_memory_list, 
                                            allocated_memory_list, 
                                                    current_process);
                                                                               
                // Run next process.
                run_process(free_memory_list, allocated_memory_list, 
                            process_list, &current_process, time);

                    
                printf("time %d, %d running, numprocesses=%d, ",
                        (int)time, current_process->process_id,
                            get_list_size(allocated_memory_list));
                                
                printf("numholes=%d, memusage=%d%%\n", 
                         get_list_size(free_memory_list),
                           memory_usage(get_used_memory(allocated_memory_list),
                              memsize));
                                                
            } else {
                // Current process has not completed so we keep running.                             
                current_process->job_time--;
                time++;                            
            }
        }        
        
    } else if (strcmp(algorithm, MULTI) == 0) {
        /***********************************
        ** Multi-Level Feedback 
        ***********************************/
        
        // Insert the processes into a queue
        insert_processes_to_list(processes, process_list);        

        // Checks if there are any new processes created, and if there are,
        // we add them to Q1.
        parse_next_processes(process_list, next_processes_list, q1, time);
        
        
        while(!processes_complete)
        {
            // "Priority" : Q1 > Q2 > Q3.
            if(is_empty(process_list) &&
                is_empty(q1) && is_empty(q2) && is_empty(q3))
            {
                // All processes in all queues have completed running
                processes_complete = 1;
                printf("time %d, simulation finished.", (int)time);
                
            } 
             else if (is_empty(q1) && is_empty(q2))
            {
                // If quantum 1 and 2 is empty, we work on quantum 3
                multi_scheduler(free_memory_list, allocated_memory_list, 
                                    process_list, next_processes_list, 
                                        q3, q1, q2, q3, &time, memsize);
            } else if (is_empty(q1))
            {
                // If quantum 1 is empty, we work on quantum 2
                multi_scheduler(free_memory_list, allocated_memory_list, 
                                    process_list, next_processes_list, 
                                        q2, q1, q2, q3, &time, memsize);
            } else {
                // Work on quantum 1
                multi_scheduler(free_memory_list, allocated_memory_list, 
                                    process_list, next_processes_list, 
                                        q1, q1, q2, q3, &time, memsize);
                
            }
        }          
    }
    
    return 0;
}


/***********************************
** Helper Functions
***********************************/

// We simply insert the process data into the list and we're done!
void
insert_processes_to_list(processes_t* processes, struct list *process_list)
{    
    int i;
    for(i=0; i < processes->number_of_processes; i++)
    {
        list_insert(process_list, &(processes->process[i]));
    }
}


// We get the next process. Using a process_data_t** effectively transfers
// information of the current process in dual directions.
void
run_process(struct list* free_memory_list, struct list* allocated_memory_list, 
                struct list* process_list, process_data_t **current_process,
                     time_t time)
{

    *current_process = (process_data_t*)(list_remove(process_list)->data);

    if(!is_process_in_memory(allocated_memory_list, *current_process))
    {
        // Requesting memory for that process
        request_memory(free_memory_list, allocated_memory_list, 
                            process_list, *current_process, time);                               
    }    
}

// Get the memory usage in percent
int
memory_usage(int n1, int n2){
    return ceil((n1/(double)n2)*DIVISOR);
}

