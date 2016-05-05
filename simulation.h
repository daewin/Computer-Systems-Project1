/********************************************************
 * Simulation Header
 * Subject: COMP30023 Computer Systems
 * Author: Daewin SV Lingam
 * Student ID: 679182
 * Date: 18/4/2016
 ********************************************************/
 
#define FCFS "fcfs"
#define MULTI "multi"

#define PRIORITY 1
#define NOT_PRIORITY 0

#define DIVISOR 100

#define QUANTUM_Q1 2
#define QUANTUM_Q2 4
#define QUANTUM_Q3 8

#include "memory.h"



void getOptions(int argc, char* argv[], char** process_file_name, char** algorithm, int *memsize);

void insert_processes_to_list(processes_t* processes, struct list *process_list);

                                   

void run_process(struct list* free_memory_list, struct list* allocated_memory_list, 
                struct list* process_list, process_data_t **current_process, time_t time);
                
int memory_usage(int n1, int n2);

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