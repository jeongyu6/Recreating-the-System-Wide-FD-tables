// #include <string.h>
// #include <stdbool.h>
#include "pid.h"
#include "utility.h"
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

 
PID_INFO* tempPIDINFO(int pid){
  ///_|> descry: the is function creates a temporary PID struct 
  ///_|> pid: value of the PID
  ///_|> returning: the PID_INFO* struct containing the PID that contains a random 
  //size of FILE_INFO struct.
    PID_INFO *temp = malloc(sizeof(PID_INFO));
    if(temp == NULL){
        perror("Failed to allocate memory");
        return NULL;
    }
    temp->pid = pid;
    temp->fd_count = 0;
    temp->fd_capacity = 20; //Note: random value I chose
    temp->fd_array = (FILE_INFO*) malloc(sizeof(FILE_INFO)*temp->fd_capacity);
    if(temp->fd_array == NULL){
        perror("Failed to allocate memory");
        // free(temp);
        return NULL;
    }
    return temp;
}


PID_INFO ** findAllPID(int *size){
  ///_|> descry: this function creates a pid_list of all the PIDs
  ///_|> size : size of the pid_list
  ///_|> returning: the PID_INFO** struct containing the address of all the PID_INFO* that 
  //contains all the PID_INFO* with different pid information
    DIR *dir = opendir("/proc");
    if(dir == NULL){
        perror("Failed to open directory");
        return NULL;
    }
    struct dirent *dirent;
    int actual_size = 0;
    int pidListCapacity = *size;
    PID_INFO ** pid_list = (PID_INFO **)malloc(sizeof(PID_INFO*) * pidListCapacity);
    if(pid_list == NULL){
        perror("Failed to allocate space for pid_list");
        closedir(dir);
        return NULL;
    }
    while((dirent = readdir(dir)) != NULL){
      char *remaining_char;
      int pid = (int)strtol(dirent->d_name,&remaining_char,10);
      
      if(*remaining_char != '\0'){
           continue;
      }
      else{
          PID_INFO * pidInfo = tempPIDINFO(pid);
          if(pidInfo == NULL){
            continue;
          }
          if(ProcessFD(pidInfo) == 1){
            if(actual_size == pidListCapacity){
                pidListCapacity *= 2;
                PID_INFO ** temp = realloc(pid_list, sizeof(PID_INFO*) *pidListCapacity);
                if(temp == NULL){
                     perror("Error: FindAllPID failed");
                     break;
                }
                pid_list = temp;
            }
            pid_list[actual_size] = pidInfo;
            actual_size++;
          }
          else{
            free(pidInfo->fd_array);
            free(pidInfo);
          }
      }
    }
    if(closedir(dir) == -1){
         perror("Failed to close file");
    }
    *size = actual_size;
    return pid_list;
}
 
