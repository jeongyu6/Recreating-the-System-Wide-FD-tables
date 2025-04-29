// #include <errno.h>
// #include <string.h> 
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/stat.h>
#include "utility.h"


int ProcessFD(PID_INFO *pid_info){
     ///_|> descry: Calculates the total number of file descriptors
     ///_|> for each PID using '/proc/<PID>/fd' directory
     ///_|> pid_info: a struct containing a specific pid number
     ///_|> returning: this function returns a 0 if an error occured
     ///_|>                                   1 if successful
     char dir_name[1024];
     snprintf(dir_name, sizeof(dir_name),"/proc/%d/fd",pid_info->pid);
     
     int success = 0;

     DIR *dir = opendir(dir_name);
     if(dir == NULL){
          return success;
     }
     struct dirent *dirent;
     while((dirent = readdir(dir)) != NULL){
          char *remaining_char;
          int FD = (int)strtol(dirent->d_name,&remaining_char,10);
          if(*remaining_char != '\0'){
               continue;
          }
          else{
               pid_info->fd_array[pid_info->fd_count].FD = FD;
               SystemWideFD(pid_info);
               (pid_info->fd_count)++;
               if((pid_info->fd_count) == pid_info->fd_capacity){
                    pid_info ->fd_capacity *= 2;
                    FILE_INFO *temp = realloc(pid_info->fd_array,sizeof(FILE_INFO) * pid_info->fd_capacity);
                    if(temp == NULL){
                         perror("Error: Memory allocation failed");
                         //error happened
                         return 0;
                    }
                    pid_info->fd_array = temp;
               }
          }
     }
     if(closedir(dir) == -1){
          perror("Failed to close file");
     }
     return 1;
}

void SystemWideFD(PID_INFO *pid_info){
     ///_|> descry: Finds the filename for the specific file descriptor
     ///_|> for each file descriptor and PID using '/proc/<PID>/fd/<FD>' directory
     ///_|> pid_info: a struct containing a specific pid number and the file descriptor
     ///_|> returning: this function does not return anything
     char path_name[1024];
     int pid = pid_info->pid;
     int current_index = pid_info->fd_count;
     int file = pid_info->fd_array[current_index].FD;
     snprintf(path_name, sizeof(path_name),"/proc/%d/fd/%d",pid,file);
     int len;
     char filename[1024];

     if((len = readlink(path_name,filename, sizeof(filename))) == -1){
          return;
     }
     
     filename[len] = '\0';
     snprintf(pid_info->fd_array[current_index].filename,
          sizeof(pid_info->fd_array[current_index].filename), "%s", filename);

     pid_info->fd_array[current_index].filename[sizeof(pid_info->fd_array[current_index].filename) - 1] = '\0';
  
     Vnode(pid_info, pid_info->fd_count);
     return;
}

void Vnode(PID_INFO *pid_info, int index){
     ///_|> descry: Calculates the Vnode for each target file 
     ///_|> for each file descriptor and PID using '/proc/<PID>/fd/<FD>' directory
     ///_|> pid_info: a struct containing a specific pid number and the file descriptor
     ///_|> index: the location of the file descriptor in the fd_array
     ///_|> returning: this function does not return anything
     struct stat file_data;
     unsigned long vnode = 0;
     const char *filename = pid_info->fd_array[index].filename;
     int pid = pid_info->pid;
     int fd = pid_info->fd_array[index].FD;
     if(stat(filename,&file_data) == 0){
        vnode = (unsigned long)file_data.st_ino;
     }

     ///_|> Resources: https://man7.org/linux/man-pages/man5/proc_pid_fdinfo.5.html 
     ///_|> This applies to sockets and is mainly used when stat() returns 0 as it is 
     ///_|> best to use /proc/PID/fdinfo/fd

     if(vnode == 0){
          char fdinfo_path[1024];
          snprintf(fdinfo_path, sizeof(fdinfo_path),"/proc/%d/fdinfo/%d", pid, fd);

          FILE *fp = fopen(fdinfo_path, "r");
          if(fp != NULL){
               char line[256];
               while(fgets(line, sizeof(line), fp)){
                    if(sscanf(line, "ino: %lu", &vnode) == 1){
                         break;
                    }
               }
               fclose(fp);
          }
     }
     pid_info->fd_array[index].inode = vnode;
 }

