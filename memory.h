/********************************************************
 * Memory Header
 * Author: Daewin SV Lingam
 * Student ID: 679182
 * Date: 15/4/2016
 ********************************************************/
 
#include "linked_list.h"
#include "process.h"

typedef struct {
    int start;
    memsize_t length;
} memory_data_t;


typedef struct {
    memory_data_t* memory_data;
    process_data_t* process_data;
    time_t time_inserted;
} allocated_memory_data_t;


void free_memory_initial(struct list* this_list, int start, memsize_t length);

void
free_process_from_memory(struct list* free_memory_list, 
                            struct list* allocated_memory_list, 
                                process_data_t* current_process);
                                
void
request_memory(struct list* free_memory_list, struct list* allocated_memory_list, 
                    struct list* process_list, process_data_t* current_process, 
                            time_t time);

void merge_adjacent_memory_check(struct list* free_memory_list);
void insertion_sort_memory(struct list* free_memory_list, memory_data_t *data);
int get_highest_free_memory(struct list* free_memory_list);   
void swap_out_process(struct list* free_memory_list, 
                struct list* allocated_memory_list, struct list* process_list);
int is_process_in_memory(struct list *allocated_memory_list, process_data_t *process);
int get_used_memory(struct list *allocated_memory_list);