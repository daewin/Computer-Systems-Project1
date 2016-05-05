/********************************************************
 * Linked List Data Structure for Project 1
 * Subject: COMP30023 Computer Systems
 * Author: Daewin SV Lingam
 * Student ID: 679182
 * Date: 9/4/2016
 ********************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.h"


// Creates a new Linked List and returns it.
// Also contains type which is either "process" or "memory".
struct list* create_list(char* type){
    struct list *new_list = malloc(sizeof(*new_list));
    
    if(new_list == NULL)
    {
        fprintf(stderr, "Error: Malloc Failed\n");
        exit(EXIT_FAILURE);
    }
    
    new_list->head = new_list->tail = NULL;
    new_list->type = type;
    
    return new_list;
}



// Inserts link to list to become the new tail
//void list_insert(struct list* this_list, process_data_t process)
void list_insert(struct list* this_list, void* data)
{
    struct node *n = malloc(sizeof(*n));
    
    if(n == NULL)
    {
        fprintf(stderr, "Error: Malloc Failed\n");
        exit(EXIT_FAILURE);
    }
    
    // Create a link based on type
    n->data = data;
    n->next = NULL;
    
    if(this_list == NULL)
    {
        // List has not been initialized
        fprintf(stderr, "Error: List has not been initialized\n");
        free(n);
        exit(EXIT_FAILURE);
        
    } else if((this_list->head == NULL) && (this_list->tail == NULL))
    {
        // First entry
        this_list->head = this_list->tail = n;
        
    } else 
    {
        // Not the first, so we add to the end
        this_list->tail->next = n;
        this_list->tail = n;
    }     
}

// Removes link from the head of the list and sets the new head accordingly.
// We also return the old head for our MULTI algorithm.
struct node* list_remove(struct list* this_list)
{
    struct node* new_head = NULL;    
    struct node* old_head = NULL;
    
    if(this_list == NULL)
    {
        // List has not been initialized
        fprintf(stderr, "Error: List has not been initialized\n");
        exit(EXIT_FAILURE);
        
    } else if((this_list->head == NULL) && (this_list->tail == NULL))
    {
        printf("List is already empty!\n");
        return NULL;
    }
    
    // Save old head
    old_head = this_list->head;
    
    // Set new_head as the next element
    new_head = this_list->head->next;
    
    // Replace current head with new_head
    this_list->head = new_head;
    
    // If this is the last element to be removed, set head and tail to NULL
    if(this_list->head == NULL)
    {
        this_list->tail = this_list->head;
    }
    
    return old_head;
}


// Checks if list is empty
int 
is_empty(struct list* this_list){
        
    if((this_list->head == NULL) && (this_list->tail == NULL)){
        return 1;
    }    
    return 0;
}



// Returns the number of lists in the linked list
int 
get_list_size(struct list* this_list)
{   
    struct node *temp = this_list->head;
    int count = 0;
    
    while(temp != NULL)
    {
        count++;    
        temp = temp->next;
    }
    
    return count;
}


// Empties the list
void
empty_list(struct list* this_list)
{
    // We do this by calling list_remove until head is NULL
    
    while(this_list->head != NULL && this_list->tail != NULL){
        list_remove(this_list);
    }
    
}
