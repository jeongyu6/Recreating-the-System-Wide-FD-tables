#ifndef UTILITY_H
#define UTILITY_H

typedef struct{
    int FD;
    char filename[1024]; 
    unsigned long inode;
}FILE_INFO;

typedef struct{
    int pid;
    int fd_count;
    int fd_capacity;
    FILE_INFO *fd_array;
}PID_INFO;

int ProcessFD(PID_INFO *pid_info);
void SystemWideFD(PID_INFO *pid_info);
void Vnode(PID_INFO *pid_info, int index);

#endif