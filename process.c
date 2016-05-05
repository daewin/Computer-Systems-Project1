/********************************************************
 * Process Data File function
 * Author: Daewin SV Lingam
 * Student ID: 679182
 * Date: 9/4/2016
 ********************************************************/
 
#include <stdio.h>
#include <stdlib.h>

#include "process.h"

processes_t* process(char* process_file_name, memsize_t memsize)
{
    int val1 = 0, val2 = 0, val3 = 0, val4 = 0;
    
    // Structure we use to build the struct of all processes
    processes_t *processes = malloc(sizeof(*processes));
    processes->number_of_processes = 0;
    
    FILE *fp;
    
    if((fp = fopen(process_file_name, "r")) == NULL)
    {
        fprintf(stderr, "Error: File does not exist!");
        exit(EXIT_FAILURE);
    }
    
    while(fscanf(fp, "%d %d %d %d", &val1, &val2, &val3, &val4) == PROCESSENTRIES)
         {
            if(memsize < val3){
                // If the process is going to take more memory than we have
                // available, we prompt and exit.
                fprintf(stderr, 
                  "Error: Process memory usage is more than available memory!");
                exit(EXIT_FAILURE);                
            }
            process_data_t *current_process = malloc(sizeof(*current_process));
            
            // Store values into new struct of process data
            current_process->time_created = val1;
            current_process->process_id = val2;
            current_process->memory_size = val3;
            current_process->job_time = val4;
                             
            processes->process[processes->number_of_processes++] = *current_process;                           
         }
         
     fclose(fp); 

     return processes;
}