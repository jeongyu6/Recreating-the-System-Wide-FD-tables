#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"
#include "pid.h"
#include "graph.h"


void print(int size){
    ///_|> descry: prints a bunch of = sign to separate between heading and the data
    ///_|> size: total number of = sign
    ///_|> returning: this function does not return anything. 

    printf("\t");
    for(int i = 0; i < size; i++){
        printf("=");
    }
    printf("\n");
}

void HandleProcessFDTable(PID_INFO ** pid_list, int size, int X){
    ///_|> descry: Indicates only the Process-FD table will be displayed
    ///_|> pid_list: list of the PID_INFO
    ///_|> size: size of the pid_list
    ///_|> X: threshold value
    ///_|> returning: this function does not return anything. 

    printf("\tPID\tFD\n");
    
    print(11);
    for(int i = 0; i < size; i++){
        for(int j = 0; j < pid_list[i]->fd_count; j++){
            if(pid_list[i]->fd_count > X){
                printf("\t%d\t%d\n", pid_list[i]->pid, j);
            }
        }
    }
    print(11);
}

void HandleSystemWideTable(PID_INFO ** pid_list, int size, int X){
    ///_|> descry: Indicates only the System-wide FD table will be displayed
    ///_|> pid_list: list of the PID_INFO
    ///_|> size: size of the pid_list
    ///_|> X: threshold value
    ///_|> returning: this function does not return anything. 
    printf("\tPID\tFD\tFilename\n");
    print(40);
    for(int i = 0; i < size; i++){
        for(int j = 0; j < pid_list[i]->fd_count; j++){
            if(pid_list[i]->fd_count > X){
                printf("\t%d\t%d\t%s\n",pid_list[i]->pid,j, pid_list[i]->fd_array[j].filename);
            }
        }
    }
    print(40);
}


void HandleVnodesTable(PID_INFO ** pid_list, int size, int X){
    ///_|> descry: Indicates only the Vnodes tables will be displayed
    ///_|> pid_list: list of the PID_INFO
    ///_|> size: size of the pid_list
    ///_|> X: threshold value
    ///_|> returning: this function does not return anything. 
     printf("\tFD\tInode\n");
     print(13);

     for(int i = 0; i < size; i++){
         for(int j = 0; j < pid_list[i]->fd_count; j++){
            if(pid_list[i]->fd_count > X){
              printf("\t%d\t%ld\n",j, pid_list[i]->fd_array[j].inode);
            }
         }
     }
     print(13);
}


void HandleCompositeTable(PID_INFO ** pid_list, int size, int X){
    ///_|> descry: Indicates only the composite table will be displayed
    ///_|> pid_list: list of the PID_INFO
    ///_|> size: size of the pid_list
    ///_|> X: threshold value
    ///_|> returning: this function does not return anything. 
     printf("\tPID\t\tFD\tFilename\t\tInode\n");
     print(53);
     int count = 0;
     for(int i = 0; i < size; i++){
         for(int j = 0; j < pid_list[i]->fd_count; j++){
            if(pid_list[i]->fd_count > X){
                printf("%-4d\t%-8d\t%-4d\t%-20s\t%ld\n", 
                count,                  
                pid_list[i]->pid,            
                j,                          
                pid_list[i]->fd_array[j].filename, 
                pid_list[i]->fd_array[j].inode);   
                count++;
                }
         }
         
     }
     print(53);

}


void HandleSummaryTable(PID_INFO ** pid_list, int size, int X){
    ///_|> descry: Indicates only the SummaryTable will be displayed
    ///_|> pid_list: list of the PID_INFO
    ///_|> size: size of the pid_list
    ///_|> X: threshold value
    ///_|> returning: this function does not return anything. 
     printf("\tSummary Table\n");
     print(11);
        for(int i = 0; i < size; i++){
            if(pid_list[i]->fd_count > X){
                printf("%d(%d), ",pid_list[i]->pid, pid_list[i]->fd_count);
            }
        }
    printf("\n");
}


void HandleThresholdTable(PID_INFO ** pid_list, int size, int X){
    ///_|> descry: Prints the HandleThresholdTable
    ///_|> pid_list: list of the PID_INFO
    ///_|> size: size of the pid_list
    ///_|> X: threshold value
    ///_|> returning: this function does not return anything. 
    printf("\n## Offending processes:\n");
    for(int i = 0; i < size; i++){
        if(pid_list[i]->fd_count > X){
           printf("%d (%d), ", pid_list[i]->pid, pid_list[i]->fd_count);
        }
    }
    printf("\n");
}


void HandleOutputTXT(PID_INFO ** pid_list, int size, int X){
    ///_|> descry: Indicates that the "composite" table in text (ASCII) format
    // will be saved in a file named compositeTable.txt
    ///_|> pid_list: list of the PID_INFO
    ///_|> size: size of the pid_list
    ///_|> X: threshold value
    ///_|> returning: this function does not return anything. 
    FILE *fp = fopen("compositeTable.txt", "w");
    if(fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "\tPID\t\tFD\tFilename\t\tInode\n");
    fprintf(fp, "\t=========================================================\n");
    int count = 0;
    for(int i = 0; i < size; i++){
        for(int j = 0; j < pid_list[i]->fd_count; j++){
            if(pid_list[i]->fd_count > X){
            fprintf(fp, "%-4d\t%-8d\t%-4d\t%-20s\t%ld\n", count,pid_list[i]->pid,j,pid_list[i]->fd_array[j].filename,pid_list[i]->fd_array[j].inode);   
            count++;
            }
        }
    }
    
    fclose(fp);

}

void HandleOutputBIN(PID_INFO ** pid_list, int size, int X){
    ///_|> descry: Indicates that the "composite" table in binary format
    // will be saved in a file named compositeTable.bin 
    ///_|> pid_list: list of the PID_INFO
    ///_|> size: size of the pid_list
    ///_|> X: threshold value
    ///_|> returning: this function does not return anything. 
    FILE *fp = fopen("compositeTable.bin", "wb");
    if(fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    int count = 0;
    CompositeTable entry;
    
    for (int i = 0; i < size; i++){
        for (int j = 0; j < pid_list[i]->fd_count; j++){
            if(pid_list[i]->fd_count > X){
            entry.composite_index = count;
            entry.pid = pid_list[i]->pid;
            entry.fd = j;
            strncpy(entry.filename, pid_list[i]->fd_array[j].filename, 20);
            entry.filename[20] = '\0';  
            entry.inode = pid_list[i]->fd_array[j].inode;
            if(fwrite(&entry, sizeof(CompositeTable), 1, fp) != 1) {
                perror("fwrite");
                fclose(fp);
                exit(EXIT_FAILURE);
            }
            count++;
            }
        }
    }
    fclose(fp);
}
