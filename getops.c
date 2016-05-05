/********************************************************
 * Get Options function adaptation
 * Author: Daewin SV Lingam
 * Student ID: 679182
 * Date: 7/4/2016
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memory.h"

/* Extern declarations: */
extern  int     optind;
extern  char    *optarg;

void
getOptions(int argc, char* argv[], char** process_file_name, char** algorithm, memsize_t *memsize)
{    
    char input;
    
    while((input = getopt(argc, argv, "f:a:m:")) != EOF)
    {
        switch(input)
        {
            case 'f':
                *process_file_name = optarg;
                break;
                
            case 'a':
                *algorithm = optarg;
                 break;
                 
            case 'm':
                *memsize = atoi(optarg);
                break;
                
            default:
                // Should not get here
                break;            
        }
    }    
}