/********************************************************
 * Memory Implementation
 * Author: Daewin SV Lingam
 * Student ID: 679182
 * Date: 15/4/2016
 ********************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#include "memory.h"


void
free_memory_initial(struct list* this_list, int start, memsize_t length){
    memory_data_t *mem = malloc(sizeof(*mem));
    
    if(mem == NULL){
        fprintf(stderr, "Error: Malloc Failed\n");
        exit(EXIT_FAILURE);
    }
    
    mem->start = start;
    mem->length = length;
    
    // Sort it in the right place
    insertion_sort_memory(this_list, mem);
}


void
free_process_from_memory(struct list* free_memory_list, 
                            struct list* allocated_memory_list, 
                                process_data_t* current_process){
      
    if(!is_process_in_memory(allocated_memory_list, current_process)){
        return;
    }
    
    int list_is_freed = 0;
    
    while(!list_is_freed){
        
        // While list is not freed, search allocated list for current process.
        // Then we remove process from allocated list, and insert memory space
        // back into free memory list.                    
        allocated_memory_data_t *allocated_memory_data = 
          (allocated_memory_data_t*)(list_remove(allocated_memory_list)->data);
        
        if(allocated_memory_data->process_data->process_id ==
                                             current_process->process_id)
        {

            // If we found the process utilizing memory
            insertion_sort_memory(free_memory_list,
                         allocated_memory_data->memory_data);
            
            // Free unused node
            free(allocated_memory_data);
            
            list_is_freed = 1;        
               
        } else {
            // If not, we insert back into the list       
            list_insert(allocated_memory_list, allocated_memory_data);
        }   
    }
}


void
request_memory(struct list* free_memory_list, struct list* allocated_memory_list, 
                    struct list* process_list, process_data_t* current_process, 
                            time_t time)
{
    // Handles all requests for memory and swapping out of processes from 
    // memory if necessary.
    
    memsize_t length = current_process->memory_size;

    while(get_highest_free_memory(free_memory_list) < length){
        // If there is no hole big enough, we swap out processes as necessary.
        swap_out_process(free_memory_list, allocated_memory_list, process_list);        
    }
    
    memory_data_t *mem = (memory_data_t*)(list_remove(free_memory_list)->data);
    
    if(mem->length < length){
        // This hole isn't big enough, so we recursively check all holes
        // to find a suitable one, then insert the memory we removed initially
        // from each stack.
        request_memory(free_memory_list, allocated_memory_list, 
                                process_list, current_process, time);
                                
        insertion_sort_memory(free_memory_list, mem);
        return;
        
    } else if (mem->length == length){
        // If it's the perfect length. 
        // Malloc for Allocated Data
        allocated_memory_data_t *allocated_memory_data = 
                            malloc(sizeof(*allocated_memory_data));
    
        allocated_memory_data->memory_data = mem;
        allocated_memory_data->process_data = current_process;
        allocated_memory_data->time_inserted = time;
        
        list_insert(allocated_memory_list, allocated_memory_data);
        return;
        
    } else if (mem->length > length){
        // Memory chunk is too big, so we split it with the excess memory
        // put back into the free list.      
        free_memory_initial(free_memory_list, 
                        (mem->start)-length, (mem->length - length));        
        
        // New allocated memory length to be just what we need, no excess.
        mem->length = length;
        
        // Malloc for Allocated Data
        allocated_memory_data_t *allocated_memory_data = 
                            malloc(sizeof(*allocated_memory_data));
        
        allocated_memory_data->memory_data = mem;
        allocated_memory_data->process_data = current_process;
        allocated_memory_data->time_inserted = time;       
        
        list_insert(allocated_memory_list, allocated_memory_data);
        return;
    } 
}

void
merge_adjacent_memory_check(struct list* free_memory_list){
    // Assert that free_memory_list has atleast one node
    assert(get_list_size(free_memory_list) > 0);
    
    // We check for adjacent memory blocks and merge them if necessary.    
    struct node* head_node = free_memory_list->head;
    struct node* next_node = free_memory_list->head->next;
    
    
    while(next_node != NULL){
        memory_data_t *head_node_data = ((memory_data_t*)(head_node->data));
        memory_data_t *next_node_data = ((memory_data_t*)(next_node->data));
        
        // Lower memory address for 'head' is compared to the upper memory
        // address for 'next', to determine if they are next to each other.
        int lower_memory_address_head = head_node_data->start
                                          - (int)(head_node_data->length);
                 
        int memory_address_next = next_node_data->start;

        if(lower_memory_address_head == memory_address_next)
        {            
            // Adjacent Memory, so we change the length of head
            memsize_t new_memory_length = head_node_data->length +
                                                 next_node_data->length;
            
            head_node_data->length = new_memory_length;
                       
            // Now we point heads 'next' to nexts' 'next', and free next.
            head_node->next = next_node->next;
            free(next_node);
            
            // Reset node pointers
            head_node = free_memory_list->head;
            next_node = free_memory_list->head->next;            
                    
        } else {
            head_node = next_node;
            next_node = next_node->next;            
        }

    }    
}
    
void
insertion_sort_memory(struct list* free_memory_list, memory_data_t *data){
    // Use insertion sort to place data in the right place. Since this is a
    // singly-linked list, we use next as our focus node and head as our
    // previous node.
    
    if(is_empty(free_memory_list)){
        // If list is already empty, we just insert.
        // Let list_insert do the rest for us.            
        list_insert(free_memory_list, data);
        return;
    }
    
    struct node* previous_node = free_memory_list->head;
    struct node* focus_node = free_memory_list->head->next;
    
    int is_sorted = 0;
    
    while(!is_sorted)
    {
        
        memory_data_t *previous_node_data = 
                ((memory_data_t*)(previous_node->data));
                
        memory_data_t *focus_node_data = NULL;
        
        if(focus_node != NULL){
            // If it's NULL, we don't store anything
           focus_node_data = ((memory_data_t*)(focus_node->data));                   
        }
         
         if(previous_node_data->start < data->start){
            // Special case of head being smaller than new data.
            // Should only be called when previous_node is the head
            // as the other case is handled by the focus node.
            struct node *new_head = malloc(sizeof(*new_head));
             
            new_head->data = data;
            new_head->next = previous_node;
             
            free_memory_list->head = new_head;
            break;     
         } 
         
         if(focus_node == NULL && previous_node_data->start > data->start){
             // If there is only one element in the list or, if we reach
             // the tail and data can't be larger than 'previous_node_data'
             // as we have checked it beforehand, we merely insert.

             list_insert(free_memory_list, data);   
             break;    
         }   

         
         if(focus_node_data->start < data->start){
             // So data should be inserted between previous and focus
             struct node *new_node = malloc(sizeof(*new_node));
             
             new_node->data = data;
             new_node->next = focus_node;
             
             previous_node->next = new_node;
             
             break;
             
         }
         
         // If we reach here, it means that data should not be sorted here.
         // Go to the next node pairs.
         previous_node = focus_node;
         focus_node = focus_node->next;       
     }        
     
    // Now merge adjacent memory
    merge_adjacent_memory_check(free_memory_list);        
}

int
get_highest_free_memory(struct list* free_memory_list){
    // Assert that list is not NULL
    assert(free_memory_list != NULL);
    
    memsize_t highest_free_memory = 0;
    
    struct node* temp_node = free_memory_list->head;
    
    while(temp_node != NULL){
        memsize_t temp_memory_size = 
                ((memory_data_t*)(temp_node->data))->length;
                
        if(temp_memory_size > highest_free_memory){
            highest_free_memory = temp_memory_size;
        }
        temp_node = temp_node->next;
    }
    
    return highest_free_memory;
}


void
swap_out_process(struct list* free_memory_list, 
                    struct list* allocated_memory_list, struct list* process_list)
{
    // We leverage the power of linked lists and add any candidates to a separate
    // list, and removing all current candidates if we find a process utilizing
    // larger memory. Then, using that list, we remove one-by-one until we find 
    // the process that has been in memory the longest.
    memsize_t highest_memory_size = 0;
    
    // Assert that allocated_memory_list is not empty
    assert(get_list_size(allocated_memory_list) > 0);
    
    // Temporary list to hold candidates 
    struct list *candidate_list = create_list("memory");    
    struct node *temporary_node = allocated_memory_list->head;
    
    while(temporary_node != NULL)
    {
        // Now we check for the highest memory usage
        allocated_memory_data_t *temp_allocated_data = 
                        (allocated_memory_data_t*)(temporary_node->data);
                        
        memory_data_t *temp_memory_data = 
                        (memory_data_t*)(temp_allocated_data->memory_data);
                        
        memsize_t temp_memory_size = temp_memory_data->length;
        
        if(temp_memory_size > highest_memory_size)
        {
            // We insert new candidate to the (emptied) list.
                       
            // Empty the list
            empty_list(candidate_list);
            
            // Insert new candidate
            list_insert(candidate_list, temp_allocated_data);
            
            highest_memory_size = temp_memory_size;
            
        } 
            else if (temp_memory_size == highest_memory_size) 
        {
            // If we have a process with the same memory usage as the current
            // candidate, we insert this candidate to the list.
            list_insert(candidate_list, temp_allocated_data);
        }
        
        // Next node
        temporary_node = temporary_node->next;    
    }
    
    
    // We now have a candidate list of processes with the largest size.
    // Find and pick the process that has been in memory the longest.
    // In this implementation, we base this on time_inserted where the longest
    // process running would have the lowest time_inserted.
    
    time_t lowest_time_inserted = INT_MAX;
    
    // Assert that candidate_list is not empty
    assert(get_list_size(candidate_list) > 0);
    
    // Re-use temporary_node
    temporary_node = candidate_list->head;
    
    while (temporary_node != NULL && get_list_size(candidate_list) != 1)
    {
        // If there is only one candidate, we have our process, else we will
        // remove a candidate first, then add them back if they are lower than
        // the current lowest_time_inserted. We should end up with one left.
        allocated_memory_data_t *temp_allocated_data = 
                        (allocated_memory_data_t*)(temporary_node->data);
        
        time_t temp_time = temp_allocated_data->time_inserted;
        
        // Remove candidate first
        list_remove(candidate_list);
        
        if(temp_time < lowest_time_inserted)
        {
            // We found a process in memory at an earlier time
            lowest_time_inserted = temp_time;
            list_insert(candidate_list, temp_allocated_data);
        }
        
        temporary_node = temporary_node->next;
    }
    
    
    // Finally, we insert the memory used by this process into our free list. //
        
    // Just making sure
    assert(get_list_size(candidate_list) == 1);
    
    // Re-using temporary_node
    temporary_node = candidate_list->head;
    
    allocated_memory_data_t *swapped_allocated_data = 
               (allocated_memory_data_t*)(temporary_node->data);
               
    // Insert used memory back to free list
    insertion_sort_memory(free_memory_list, swapped_allocated_data->memory_data);
       
    
    // Find and remove process entry from allocated_memory_list. //
    
    // Record the Process ID of our swapped out process
    process_data_t *swapped_process_data = 
                    (process_data_t*)(swapped_allocated_data->process_data);
                            
    int swapped_process_id = swapped_process_data->process_id;
    
    int swapped_out = 0;    
    while(!swapped_out)
    {       
        // Keep removing and adding it back, until we find it
        allocated_memory_data_t *allocated_memory_data = 
            (allocated_memory_data_t*)(list_remove(allocated_memory_list)->data);
                        
        if(allocated_memory_data->process_data->process_id == swapped_process_id)
        {
            // We found it. As it is already removed, we do nothing!       
            swapped_out = 1;
            
        } else {
            // Insert back into list
            list_insert(allocated_memory_list, allocated_memory_data);
        }
    }
        
    // Free candidate list and we're done!    
    free(candidate_list);   
}



// Checks if the process is currently in memory
int
is_process_in_memory(struct list *allocated_memory_list, process_data_t *process)
{
    
    struct node* temp_node = allocated_memory_list->head;
    
    while(temp_node != NULL)
    {
        allocated_memory_data_t *temp_data = (allocated_memory_data_t*)(temp_node->data); 
        
        if(process->process_id == temp_data->process_data->process_id)
        {
            return 1;
        }
        
        temp_node = temp_node->next;
    }
    
    return 0;
}   

// Returns the memory used by processes in allocated_memory_list
int
get_used_memory(struct list *allocated_memory_list)
{
    struct node* temp_node = allocated_memory_list->head;
    
    int used_memory = 0;
    
    while(temp_node != NULL)
    {
        allocated_memory_data_t *temp_data = (allocated_memory_data_t*)(temp_node->data); 
        
        used_memory += temp_data->memory_data->length;
        
        temp_node = temp_node->next;
    }
    
    return used_memory;    
}