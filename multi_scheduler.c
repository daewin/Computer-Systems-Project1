/********************************************************
 * Multi-Scheduler Function for Project 1
 * Subject: COMP30023 Computer Systems
 * Author: Daewin SV Lingam
 * Student ID: 679182
 * Date: 18/4/2016
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "simulation.h"

// Prototype Declaration
int get_quantum(struct list *qn);

void
multi_scheduler(struct list* free_memory_list, 
                    struct list* allocated_memory_list, 
                        struct list *process_list,
                            struct list *next_processes_list, struct list *qn,
                                struct list *q1, struct list *q2,
                                    struct list *q3, time_t *time, 
                                        memsize_t memsize);

void get_next_processes(struct list *process_list, 
                                    struct list *next_processes_list,
                                        time_t time);
                                        
void parse_next_processes(struct list *process_list, 
                        struct list *next_processes_list,
                            struct list *q1,
                                time_t time);


// Scheduling using round-robin with a different quantum for different queues.
void
multi_scheduler(struct list* free_memory_list, 
                    struct list* allocated_memory_list, 
                        struct list *process_list,
                            struct list *next_processes_list, struct list *qn,
                                struct list *q1, struct list *q2,
                                    struct list *q3, time_t *time, 
                                        memsize_t memsize)
{
    // The queue were are scheduling is qn. Get its quantum.
    int quantum = get_quantum(qn);   
    
    while(get_list_size(qn) > 0)
    {
        // We will tick incrementally to allow new process creation to be
        // accounted for, instead of incrementing by quantum sized time.
        int tick = 0;
        
        process_data_t *current_job = NULL;
        
        // Allocate memory to this process and run it!
        run_process(free_memory_list, allocated_memory_list, qn, &current_job, 
                                                                (time_t)*time);
        
        // Assert that there is a current job
        assert(current_job != NULL);
        
        printf("time %d, %d running, numprocesses=%d, ", (int)*time,
                    current_job->process_id, 
                        get_list_size(allocated_memory_list));
                            
        printf("numholes=%d, memusage=%d%%\n", get_list_size(free_memory_list),                    
                memory_usage(get_used_memory(allocated_memory_list), memsize));
                                                              
        
        while((tick < quantum) && (current_job->job_time > 0))
        {
            
            // Do the necessary ticking, but only if there is any more process
            // time.
            current_job->job_time--;
            *time += 1;
            
            // Time=0 has already been parsed so we do this after the increment
            parse_next_processes(process_list, next_processes_list, q1, 
                                                                (int)*time);
            
            tick++;
        }
        
        // Now, we check if this process exhausted its quantum. If it did,
        // we'll move it to a lower queue (lowest is Q3); else it's removed!        
        if(current_job->job_time > 0)
        {            
            if(strcmp(qn->type, "q1") == 0)
            {                
                // If it's in Q1 initially, move to Q2
                list_insert(q2, current_job);
                
            } else if(strcmp(qn->type, "q2") == 0)
            {
                // If it's in Q2 initially, move to Q3
                list_insert(q3, current_job);         
            } else {
                // If it's in Q3, keep it in Q3
                list_insert(q3, current_job);  
            }
        } else {
            // Free it.
            free_process_from_memory(free_memory_list, allocated_memory_list,
                                                                  current_job);
        }
        
                
        if(get_list_size(q1) > 0)
        {
            // If there is a new process created, we prioritize it and start
            // from q1 again.
            return;
        }
        
    }    
}


/***********************************
** Helper Functions
***********************************/
int
get_quantum(struct list *qn)
{
    // Set the quantum based on the queue type itself.
    
    if(strcmp(qn->type, "q1") == 0)
    {
        return QUANTUM_Q1;
    } else if(strcmp(qn->type, "q2") == 0)
    {
        return QUANTUM_Q2;
    } else if(strcmp(qn->type, "q3") == 0)
    {
        return QUANTUM_Q3;
    } else 
    {
        fprintf(stderr, "Error: Queue passed is incorrect!\n");
        exit(EXIT_FAILURE);
    }    
}


// Checks the process list if there are processes that are scheduled to run
// at a particular time. If there are, we insert them to next_processes_list.
void 
get_next_processes(struct list *process_list, 
                        struct list *next_processes_list,
                            time_t time)
{    
    struct node* temp_node = process_list->head;
    
    while(temp_node != NULL)
    {
        process_data_t *current_process = (process_data_t*)(temp_node->data);
        
        if(current_process->time_created == time)
        {
            list_insert(next_processes_list, current_process);
        }
        
        temp_node = temp_node->next;
    }
}


// Checks if there are any new processes created, and if there are, we add
// them to Q1.
void
parse_next_processes(struct list *process_list, 
                        struct list *next_processes_list,
                            struct list *q1,
                                time_t time)
{
    // Inserts all processes that are created at "time" into queue Q1
    
    get_next_processes(process_list, next_processes_list, time);
    
    while(get_list_size(next_processes_list) > 0)
    {
        // As the number of processes created is in order with regards to time
        // in our process_list, we can just remove them straight from the list.
        list_insert(q1, list_remove(process_list)->data);
        
        // And to clear next_processes_list, of course.
        list_remove(next_processes_list);
    }
}