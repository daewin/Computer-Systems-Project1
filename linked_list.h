/********************************************************
 * Linked List Header
 * Subject: COMP30023 Computer Systems
 * Author: Daewin SV Lingam
 * Student ID: 679182
 * Date: 9/4/2016
 ********************************************************/
 
struct node
{
    void* data;
    struct node* next;
};

// List of Determining Nodes and Type Identifier
struct list
{
    struct node* head;
    struct node* tail;
    char* type;
};

struct list* create_list(char* type);
void list_insert(struct list* this_list, void* data);
struct node* list_remove(struct list* this_list);
int is_empty(struct list* this_list);
int get_list_size(struct list* this_list);
void empty_list(struct list* this_list);