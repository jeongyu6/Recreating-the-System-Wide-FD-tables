// #include <errno.h> 
#include <stdbool.h>
#include <stdio.h>
#include <string.h> 
#include <dirent.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "pid.h"
#include "graph.h"

typedef struct flags{
    bool per_process;
    bool systemWide;
    bool Vnodes;
    bool composite;
    bool summary;
    int threshold;
    int specificPID;
    bool specPID;
    bool output_TXT;
    bool output_binary;
    PID_INFO ** pid_list;
    int specificIndex;
} Flags;

void handleFlags(Flags *flags){
    flags->per_process = false;
    flags->systemWide = false;
    flags->Vnodes = false;
    flags->composite = false;
    flags->summary = false;
    flags->threshold = 0;
    flags->specificPID = -1;
    flags->specPID = false;
    flags->output_TXT = false;
    flags->output_binary = false;
    flags->pid_list = NULL;
    flags->specificIndex = -1;
}


int main(int argc, char *argv[]) {
    ///_|> descry: Handles the Multiple Command Line Argument
    ///_|>This function does the following: 
    ///_|>      If 'argc == 1' and 'isdigit(argv[i][0])', then we set and process only the particular PID number
    ///_|>              if !isdigit(argv[i][0]), then we process all the PID numbers
    ///_|>      If 'argc > 2', then we check for the particular flags 
    ///_|>          ('--per-process', '--systemWide', '--Vnodes','--composite', '--summary', '--threshold=X')
    ///_|>          and ('--output_TXT', "--output_binary") for bonus marks (+2), then we draw the table depending on the flags
    ///_|> argc:total number of arguments       
    ///_|> *argv[] : array of command line arguments 
    ///_|> returning: 0 if successful -1 if error.

    Flags flag;
    handleFlags(&flag);

    ///_|> NOTES:
    ///_|> positional argument: only one positional argument indicating a
    ///_|> particular process id number (PID), if not specified the program
    ///_|> will attempt to process all the currently running processes for
    ///_|> the user executing the program

    int i = 1;
    if(i < argc && isdigit(argv[i][0])){
        flag.specificPID = atoi(argv[i]);
        flag.specPID = true;
        i++;
    }
    for(; i < argc; i++){
        if(strncmp(argv[i], "--per-process", 13) == 0){
            flag.per_process = true;
            // printf("per process\n");
        }
        else if(strncmp(argv[i], "--systemWide",12) == 0){
            flag.systemWide = true;
            // printf("system wide\n");
        }
        else if(strncmp(argv[i], "--Vnodes",7) == 0){
            flag.Vnodes = true;
            // printf("vnodes\n");
        }
        else if(strncmp(argv[i], "--composite",11) == 0){
            flag.composite = true;
            // printf("composite\n");
        }
        else if(strncmp(argv[i], "--summary",9) == 0){
            flag.summary = true;
            // printf("summary\n");
        }
        else if(strncmp(argv[i], "--threshold=",12) == 0){
            char *number = argv[i] + 12;
            // printf("threshold\n");
            if(!isdigit(number[0])){
                fprintf(stderr, "Error: %s is not a number\n", number);
                exit(EXIT_FAILURE);
            }
            char *remaining_char;
            int X = (int)strtol(number, &remaining_char, 10);
            if(*remaining_char != '\0'){
                fprintf(stderr, "Error: Some characters are not numeric: %s\n", remaining_char);
                exit(EXIT_FAILURE);
            }
            if(X < 0){
                fprintf(stderr, "Error: Cannot be a negative number");
            }
            flag.threshold = X;
            // printf("%d\n", X);
        }
        else if(strcmp(argv[i], "--output_TXT") == 0){
            flag.output_TXT = true;
        }
        else if(strcmp(argv[i], "--output_binary") == 0){
            flag.output_binary = true;
        }
        else{
            fprintf(stderr, "Invalid argument %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }

    ///_> NOTE: If no argument is passed, the program will display the composite table (my decision)
    if(!flag.composite && !flag.per_process && !flag.systemWide && !flag.Vnodes && !flag.threshold 
        && !flag.summary && !flag.output_binary && !flag.output_TXT){
        flag.composite = true;
    }
 
    int size = 10;
    flag.pid_list = findAllPID(&size);
    if(flag.pid_list == NULL){
        perror("Failed to allocate list_of_PIDS");
        exit(EXIT_FAILURE);
    }

    if(flag.specPID){
        int index = -1;
        for(int j = 0; j < size; j++){
            if(flag.pid_list[j] != NULL && flag.pid_list[j]->pid == flag.specificPID){
                index = j;
                break;
            }
        }
        if(index == -1){
            fprintf(stderr, "Specific PID %d not found in the list\n", flag.specificPID);
            exit(EXIT_FAILURE);
        }
        else{
            printf(">>> Target PID: %d\n\n", flag.specificPID);
        }
        flag.specificIndex = index;
    }

    ///_|> At this point, process list_of_PIDS according to your flags.
    ///_|> Printing, further processing, etc.)

    if(flag.per_process){
        if(flag.specPID)
            HandleProcessFDTable(&flag.pid_list[flag.specificIndex], 1, flag.threshold);
        else
            HandleProcessFDTable(flag.pid_list, size, flag.threshold);
    }
    if(flag.systemWide){
        if(flag.specPID)
            HandleSystemWideTable(&flag.pid_list[flag.specificIndex], 1, flag.threshold);
        else
            HandleSystemWideTable(flag.pid_list, size, flag.threshold);
    }
    if(flag.Vnodes){
        if(flag.specPID)
            HandleVnodesTable(&flag.pid_list[flag.specificIndex], 1, flag.threshold);
        else
            HandleVnodesTable(flag.pid_list, size, flag.threshold);
    }
    if(flag.composite){
        if(flag.specPID)
            HandleCompositeTable(&flag.pid_list[flag.specificIndex], 1, flag.threshold);
        else
            HandleCompositeTable(flag.pid_list, size, flag.threshold);
    }
    if(flag.summary){
        HandleSummaryTable(flag.pid_list, size, flag.threshold);
    }
    if(flag.threshold){
        HandleThresholdTable(flag.pid_list, size, flag.threshold);
    }
    
    if(flag.output_TXT){
        HandleOutputTXT(flag.pid_list, size, flag.threshold);
    }
    
    if(flag.output_binary){
        HandleOutputBIN(flag.pid_list, size, flag.threshold);
    }

    for (int i = 0; i < size; i++) {
        if (flag.pid_list[i] != NULL) {
            free(flag.pid_list[i]->fd_array);
            free(flag.pid_list[i]);
        }
    }
    free(flag.pid_list);

    return 0;
}


    /**
     * Back Logic: Scratch (Ignore this)
    // 0. Call handleFlags() function, which returns Flags struct. 
    // 1. create a list of PIDs = list_of_pids -> (int* for an array, int for size of array)
    // 2. If specific PID is given, call doesExist() function. If exist, insert that pid in list_of_pids. 
    // Else, let the list_of_pids be empty
    // 3. if specific PID is not given, call findAllPid() function, which will store all the pid in list_of_pids
    4. have if blocks like: if (flag.per_process) {call HandleProcessFDTable(list_of_pids)}, if (flag.composite) {call handleCompositeTable(list_of_pids)}
    //not given access to the main

     PerProcessTable.c
    n. HandlePerProcessTable(): 
    printHeader()
    for pid in list_of_pids:
        list_of_fds = util.mapPidToFds(pid)
        for fd in list_of_fds:
            printRow(pid, fd)

    printFooter()                                


    compositeTable.c
    x. HandleCompositeTable.(): ffor pid in list_of_pids:
                                    list_of_fds = util.mapPidToFds(pid)




    util.c => MapPidToFds() -> Gives you fds
    */
