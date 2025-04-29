#ifndef GRAPH_H
#define GRAPH_H

typedef struct {
    int composite_index;
    int pid;
    int fd;
    char filename[21];  // fixed-length field for the filename (20 chars + null)
    long inode;
} CompositeTable;


void HandleProcessFDTable(PID_INFO ** pid_list, int size, int X);
void HandleSystemWideTable(PID_INFO ** pid_list, int size, int X);
void HandleVnodesTable(PID_INFO ** pid_list, int size, int X);
void HandleCompositeTable(PID_INFO ** pid_list, int size, int X);
void HandleSummaryTable(PID_INFO ** pid_list, int size, int X);
void HandleThresholdTable(PID_INFO ** pid_list, int size, int X);
void HandleOutputTXT(PID_INFO ** pid_list, int size, int X);
void HandleOutputBIN(PID_INFO ** pid_list, int size, int X);
#endif