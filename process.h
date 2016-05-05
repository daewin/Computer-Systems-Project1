/********************************************************
 * Process Data File Header
 * Author: Daewin SV Lingam
 * Student ID: 679182
 * Date: 18/4/2016
 ********************************************************/
 
#define PROCESSLIMIT 1000
#define PROCESSENTRIES 4

typedef int memsize_t;

typedef struct {
    int time_created;
    int process_id;
    memsize_t memory_size;
    int job_time;
} process_data_t;

typedef struct {
    // We assume that the maximum number of processes simulated is 1000
    // for simplification
    process_data_t process[PROCESSLIMIT];
    int number_of_processes;
} processes_t;


processes_t* process(char* process_file_name, memsize_t memsize);